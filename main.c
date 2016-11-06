#include <nds.h>
#include <filesystem.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

int main(void) {
	consoleDemoInit();

	if (nitroFSInit(NULL)) {
		FILE* model_file = fopen("ico.dae","rb");
		if (model_file) {
			iprintf("File found :)\n");
		} else {
			iprintf("File not found :(\n");
		}
	}

	while(1) {
		swiWaitForVBlank();
	}

	return 0;
}
