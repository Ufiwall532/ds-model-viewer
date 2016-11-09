#ifndef _MODEL_H_
#define _MODEL_H_

struct model {
	struct geometry *geometries;
	int count;
};

struct geometry {
    double *vertices;
	int *indices;
	int count;
	m4x4 matrix;
};

struct model *load_model(char *file_name);

#endif

