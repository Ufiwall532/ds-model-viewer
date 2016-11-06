#include <nds.h>
#include <filesystem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "simplexml.h"

void* float_array_content_xml_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char* szName, const char* szAttribute, const char* szValue)
{
	if(event == ADD_ATTRIBUTE) {
		if (strcmp(szAttribute, "id") == 0) {
			if(strstr(szValue, "mesh-positions-array")) {
				simpleXmlPushUserData(parser, "mesh-positions-array");
			}
		}
	} else if(event == ADD_CONTENT) {
		if(strcmp("mesh-positions-array", (char *)simpleXmlPopUserData(parser)) == 0) {
			iprintf("%s", szValue);
		}
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

void* source_xml_handler(SimpleXmlParser parser, SimpleXmlEvent event,
	const char* szName, const char* szAttribute, const char* szValue)
{
	if (event == ADD_SUBTAG) {
		if (strcmp(szName, "source") == 0) {
			return float_array_xml_handler;
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
							iprintf("Parsed xml file\n");
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

	while(1) {
		swiWaitForVBlank();
	}

	return 0;
}

