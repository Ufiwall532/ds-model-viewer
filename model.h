#ifndef _MODEL_H_
#define _MODEL_H_

struct model {
	struct mesh *meshes;
	int count;
};

struct vector3 {
    double x;
    double y;
    double z;
};

struct mesh {
    struct vector3 *vertices;
    int *indices;
	int count;
};

struct model *load_model(char *file_name);

#endif

