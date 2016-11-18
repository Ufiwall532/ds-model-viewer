#include <nds.h>
#include <filesystem.h>
#include <stdio.h>
#include "model.h"

struct model *load_model(char *file_name) {
	FILE *file = fopen(file_name, "r");
	
	if(!file) {
	    iprintf("Failed to open file %s.\n", file_name);
	    return NULL;   
	}
	
	int mesh_count = 0;
	if(fscanf(file, "%i", &mesh_count) != 1) {
	    iprintf("Could not read how many mesh objects to load.\n");
	    return NULL;
	}
	
    struct model *new_model = calloc(1, sizeof(struct model));
    new_model->count = mesh_count;
	new_model->meshes = malloc(mesh_count * sizeof(struct mesh));
	
	for(int i = 0; i < mesh_count; i++) {
	    int vertex_count = 0;
	    fscanf(file, "%i", &vertex_count);
	    new_model->meshes[i].vertices = malloc(vertex_count * sizeof(struct vector3));

	    for(int j = 0; j < vertex_count; j++) {
	        double x, y, z;
	        fscanf(file, "%lf %lf %lf", &x, &y, &z);
	        new_model->meshes[i].vertices[j].x = x;
	        new_model->meshes[i].vertices[j].y = y;
	        new_model->meshes[i].vertices[j].z = z;
	    }
	    
	    int index_count = 0;
	    fscanf(file, "%d", &index_count);
	    new_model->meshes[i].count = index_count;
	    new_model->meshes[i].indices = malloc(index_count * 3 * sizeof(int));
	
	    for(int j = 0; j < index_count; j++) {
	        int v1, v2, v3;
	        fscanf(file, "%i %i %i", &v1, &v2, &v3);
	        new_model->meshes[i].indices[j * 3] = v1;
	        new_model->meshes[i].indices[j * 3 + 1] = v2;
	        new_model->meshes[i].indices[j * 3 + 2] = v3;
	    }
	}
	
	return new_model;
}

