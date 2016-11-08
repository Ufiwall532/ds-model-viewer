#include <nds.h>
#include <filesystem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "simplexml.h"

enum model_load_state {INIT, START_MESH_POSITION, STOP_MESH_POSITION};
enum model_load_state state = INIT;

double *mesh_positions;
int *poly_positions;
int poly_count;

void* float_array_content_xml_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char* szName, const char* szAttribute, const char* szValue)
{
	if(event == ADD_ATTRIBUTE) {
		if (strcmp(szAttribute, "id") == 0) {
			if(strstr(szValue, "mesh-positions-array")) {
				state = START_MESH_POSITION;
			}
		} else if (strcmp(szAttribute, "count") == 0) {
			if(state == START_MESH_POSITION) {
				mesh_positions = malloc(atoi(szValue) * sizeof(double));
			}
		}
	} else if(event == ADD_CONTENT) {
		if(state == START_MESH_POSITION) {
			char *position = strtok((char *)szValue, " ");
			int i = 0;
			while(position) {
				mesh_positions[i++] = atof(position);
				position = strtok(NULL, " ");
			}
			iprintf("mesh positions count %i\n", i);
			state = STOP_MESH_POSITION;
		}
	}
	return NULL;
}

void* p_content_xml_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char* szName, const char* szAttribute, const char* szValue)
{
	if(event == ADD_CONTENT) {
		char *position = strtok((char *)szValue, " ");
		int i = 0, m = 0;
		while(position) {
			// read every other position since polylist contains normal data
			if(m++ % 2 == 0) {
				poly_positions[i++] = atoi(position);
			}
			position = strtok(NULL, " ");
		}
		iprintf("poly index count %i\n", i);
	}
	return NULL;
}

void* float_array_xml_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char* szName, const char* szAttribute, const char* szValue)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(szName, "float_array") == 0) {
			return float_array_content_xml_handler;
		}
	}
	return NULL;
}

void* polylist_xml_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char* szName, const char* szAttribute, const char* szValue)
{
	if(event == ADD_ATTRIBUTE) {
		if (strcmp(szAttribute, "count") == 0) {
			poly_count = atoi(szValue) * 3; // triangulated
			poly_positions = malloc(poly_count * sizeof(int));
		}
	} else if (event == ADD_SUBTAG) {
		if (strcmp(szName, "p") == 0) {
			return p_content_xml_handler;
		}
	}
	return NULL;
}

void* source_xml_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char* szName, const char* szAttribute, const char* szValue)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(szName, "source") == 0) {
			return float_array_xml_handler;
		} else if (strcmp(szName, "polylist") == 0) {
			return polylist_xml_handler;
		}
	}
	return NULL;
}

void* mesh_xml_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char* szName, const char* szAttribute, const char* szValue)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(szName, "mesh") == 0) {
			return source_xml_handler;
		}
	}
	return NULL;
}

void* geometries_xml_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char* szName, const char* szAttribute, const char* szValue)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(szName, "geometry") == 0) {
			return mesh_xml_handler;
		}
	}
	return NULL;
}

void* library_geometries_xml_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char* szName, const char* szAttribute, const char* szValue)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(szName, "library_geometries") == 0) {
			return geometries_xml_handler;
		}
	}
	return NULL;
}

void* collada_xml_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char* szName, const char* szAttribute, const char* szValue)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(szName, "COLLADA") == 0) {
			return library_geometries_xml_handler;
		}
	}
	return NULL;
}

int main(void) {
	consoleDemoInit();

	if (nitroFSInit(NULL)) {
		FILE* model_file = fopen("ico.dae","r");
		if (model_file) {
			iprintf("File found :)\n");
			char* file_data;
			struct stat file_stat;
			if(stat("ico.dae", &file_stat) != 0) {
				iprintf("Failed getting info about the file\n");
			} else {
				file_data = malloc(file_stat.st_size + 1);
				if(!file_data) {
					iprintf("Not enough memory to load file\n");
				} else {
					size_t bytes_read = fread(file_data, sizeof(char), file_stat.st_size, model_file);
					if(bytes_read == file_stat.st_size) {
						iprintf("Read all %d bytes into memory!\n", bytes_read);
						SimpleXmlParser parser = simpleXmlCreateParser(file_data, bytes_read);
						if(parser) {
							simpleXmlParse(parser, collada_xml_handler);
							simpleXmlDestroyParser(parser);
						}
					}
				}
				free(file_data);
			}
		} else {
			iprintf("File not found :(\n");
		}
		fclose(model_file);
	}
	
	videoSetMode(MODE_0_3D);
	glInit();
	glClearColor(0,0,0,31); // BG must be opaque for AA to work
	glClearPolyID(63); // BG must have a unique polygon ID for AA to work
	glClearDepth(0x7FFF);
	glViewport(0,0,255,191);
	
	//any floating point gl call is being converted to fixed prior to being implemented
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 256.0 / 192.0, 0.1, 40);
	
	gluLookAt(	0.0, 0.0, 1.0,		//camera possition 
				0.0, 0.0, 0.0,		//look at
				0.0, 1.0, 0.0);		//up

	float rotateX = 0.0;
	float rotateY = 0.0;

	while(1) {
		glPushMatrix();
		swiWaitForVBlank();
		glTranslatef32(0, 0, floattof32(-1));
		glRotateX(rotateX);
		glRotateY(rotateY);
		glMatrixMode(GL_MODELVIEW);
		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);
		
		for(int i = 0; i < poly_count; i += 3) {
			glBegin(GL_TRIANGLE);
				glColor3b(255,0,0);
				glVertex3v16(
					floattof32(mesh_positions[poly_positions[i] * 3]),
					floattof32(mesh_positions[poly_positions[i] * 3 + 1]),
					floattof32(mesh_positions[poly_positions[i] * 3 + 2])
				);

				glColor3b(0,255,0);
				glVertex3v16(
					floattof32(mesh_positions[poly_positions[i + 1] * 3]),
					floattof32(mesh_positions[poly_positions[i + 1] * 3 + 1]),
					floattof32(mesh_positions[poly_positions[i + 1] * 3 + 2])
				);

				glColor3b(0,0,255);
				glVertex3v16(
					floattof32(mesh_positions[poly_positions[i + 2] * 3]),
					floattof32(mesh_positions[poly_positions[i + 2] * 3 + 1]),
					floattof32(mesh_positions[poly_positions[i + 2] * 3 + 2])
				);
			glEnd();
		}
		
		glPopMatrix(1);
		glFlush(0);
		
		rotateX += 2;
		rotateY -= 1;
	}

	return 0;
}

