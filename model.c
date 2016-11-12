#include <nds.h>
#include <filesystem.h>
#include <stdio.h>
#include <sys/stat.h>
#include "simplexml.h"
#include "model.h"

enum model_load_state {INIT, START_MESH_POSITION, STOP_MESH_POSITION};
enum model_load_state state = INIT;

struct model *new_model;

void *float_array_content_tag_handler(SimpleXmlParser parser,
    SimpleXmlEvent event, const char *name, const char *attribute,
    const char *value)
{
    struct geometry *curr = &new_model->geometries[new_model->count-1];
    
	if(event == ADD_ATTRIBUTE) {
		if (strcmp(attribute, "id") == 0) {
			if(strstr(value, "mesh-positions-array")) {
				state = START_MESH_POSITION;
			}
		} else if (strcmp(attribute, "count") == 0) {
			if(state == START_MESH_POSITION) {
				curr->vertices = malloc(atoi(value) * sizeof(double));
			}
		}
	} else if(event == ADD_CONTENT) {
		if(state == START_MESH_POSITION) {
			char *position = strtok((char *)value, " ");
			int i = 0;
			while(position) {
				curr->vertices[i++] = atof(position);
				position = strtok(NULL, " ");
			}
			state = STOP_MESH_POSITION;
		}
	}
	return NULL;
}

void *p_content_tag_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char *name, const char *attribute, const char *value)
{
	if(event == ADD_CONTENT) {
	    struct geometry *curr = &new_model->geometries[new_model->count-1];
		char *position = strtok((char *)value, " ");
		int i = 0, m = 0;
		while(position) {
			// read every other position since polylist contains normal data
			if(m++ % 2 == 0) {
				curr->indices[i++] = atoi(position);
			}
			position = strtok(NULL, " ");
		}
	}
	return NULL;
}

void *float_array_tag_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char *name, const char *attribute, const char *value)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(name, "float_array") == 0) {
			return float_array_content_tag_handler;
		}
	}
	return NULL;
}

void *polylist_tag_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char *name, const char *attribute, const char *value)
{
	if(event == ADD_ATTRIBUTE) {
		if (strcmp(attribute, "count") == 0) {
		    struct geometry *curr = &new_model->geometries[new_model->count-1];
			curr->count = atoi(value) * 3;
			curr->indices = malloc(curr->count * sizeof(int));
		}
	} else if (event == ADD_SUBTAG) {
		if (strcmp(name, "p") == 0) {
			return p_content_tag_handler;
		}
	}
	return NULL;
}

void *source_tag_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char *name, const char *attribute, const char *value)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(name, "source") == 0) {
			return float_array_tag_handler;
		} else if (strcmp(name, "polylist") == 0) {
			return polylist_tag_handler;
		}
	}
	return NULL;
}

void *mesh_tag_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char *name, const char *attribute, const char *value)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(name, "mesh") == 0) {
			return source_tag_handler;
		}
	}
	return NULL;
}

void *matrix_content_tag_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char *name, const char *attribute, const char *value)
{
    static int matrix_count = 0;
	if (event == ADD_CONTENT) {
	    struct geometry *curr = &new_model->geometries[matrix_count++];
	    char *position = strtok((char *)value, " ");
	    double m_copy[16];
		int i = 0;
		
		while(position) {
			m_copy[i++] = floattov16(atof(position));
			position = strtok(NULL, " ");
		}
		
		curr->matrix.m[0] = m_copy[0];
		curr->matrix.m[1] = m_copy[4];
		curr->matrix.m[2] = m_copy[8];
		curr->matrix.m[3] = m_copy[12];
		curr->matrix.m[4] = m_copy[1];
		curr->matrix.m[5] = m_copy[5];
		curr->matrix.m[6] = m_copy[9];
		curr->matrix.m[7] = m_copy[13];
		curr->matrix.m[8] = m_copy[2];
		curr->matrix.m[9] = m_copy[6];
		curr->matrix.m[10] = m_copy[10];
		curr->matrix.m[11] = m_copy[14];
		curr->matrix.m[12] = m_copy[3];
		curr->matrix.m[13] = m_copy[7];
		curr->matrix.m[14] = m_copy[11];
		curr->matrix.m[15] = m_copy[15];
	}
	return NULL;
}

void *matrix_tag_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char *name, const char *attribute, const char *value)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(name, "matrix") == 0) {
		    return matrix_content_tag_handler;
		}
	}
	return NULL;
}

void *node_tag_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char *name, const char *attribute, const char *value)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(name, "node") == 0) {
			return matrix_tag_handler;
		}
	}
	return NULL;
}

void *visual_scene_tag_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char *name, const char *attribute, const char *value)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(name, "visual_scene") == 0) {
			return node_tag_handler;
		}
	}
	return NULL;
}

void *geometry_tag_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char *name, const char *attribute, const char *value)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(name, "geometry") == 0) {
		    new_model->count++;
		    new_model->geometries = realloc(new_model->geometries,
		        new_model->count * sizeof(struct geometry));		    
			return mesh_tag_handler;
		}
	}
	return NULL;
}

void *library_geometries_tag_handler(SimpleXmlParser parser, 
    SimpleXmlEvent event, const char *name, const char *attribute,
    const char *value)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(name, "library_geometries") == 0) {
			return geometry_tag_handler;
		} else if (strcmp(name, "library_visual_scenes") == 0) {
			return visual_scene_tag_handler;
		}
	}
	return NULL;
}

void *collada_tag_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char *name, const char *attribute, const char *value)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(name, "COLLADA") == 0) {
			return library_geometries_tag_handler;
		}
	}
	return NULL;
}

struct model *load_model(char *file_name) {
	struct stat file_stat;
	
	if(stat(file_name, &file_stat) != 0) {
		iprintf("Failed to load stat info about %s.\n", file_name);
		return NULL;
	}
	
	char *file_data;
	file_data = malloc(file_stat.st_size + 1);
	
	if(!file_data) {
	    iprintf("Not enough memory to load file %s.\n", file_name);
	    return NULL;
	}
	
	FILE *file = fopen(file_name, "r");
	
	if(!file) {
	    iprintf("Failed to open file %s.\n", file_name);
	    return NULL;   
	}
	
	size_t bytes_read = fread(file_data, sizeof(char), file_stat.st_size, file);
    fclose(file);
    
    if(bytes_read != file_stat.st_size) {
	    iprintf("Could not read all bytes for %s.\n", file_name);
	    return NULL;
	}    
	
	SimpleXmlParser xml_parser = simpleXmlCreateParser(file_data, bytes_read);
	
	if(!xml_parser) {
	    iprintf("Could not create xml parser for %s.\n", file_name);
	    return NULL;
	}

    new_model = calloc(1, sizeof(struct model));
	simpleXmlParse(xml_parser, collada_tag_handler);
	simpleXmlDestroyParser(xml_parser);
	free(file_data);
	
	return new_model;
}

