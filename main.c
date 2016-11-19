#include <nds.h>
#include <filesystem.h>
#include <stdio.h>
#include <nds/arm9/image.h>
#include <sys/stat.h>
#include "model.h"

int texture[1];

int LoadGLTextures() {
    sImage pcx;
    struct stat sb;
    unsigned char *contents;
    
    if(stat("diffuse.pcx", &sb) != 0) {
        iprintf("Failed to get stat.\n");
        return FALSE;
    }

    FILE *file = fopen("diffuse.pcx", "rb");
    contents = (unsigned char *)malloc(sb.st_size + 1);
    fread(contents, sizeof(unsigned char), sb.st_size, file);

    if(!file) {
        iprintf("Failed to open file.\n");
        return FALSE;   
    }

    loadPCX((u8*)contents, &pcx);	
    image8to16(&pcx);

    glGenTextures(1, &texture[0]);
    glBindTexture(0, texture[0]);
    glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_128, TEXTURE_SIZE_128, 0,
        TEXGEN_TEXCOORD, pcx.image.data8);

    imageDestroy(&pcx);
    fclose(file);

    return TRUE;
}

int main(void) {
    consoleDemoInit();

    if (!nitroFSInit(NULL)) {
        iprintf("Error could not initialize file system.\n");
        return 0;
    }
	
    struct model *model = load_model("star.sos");
	
    videoSetMode(MODE_0_3D);
    vramSetBankA(VRAM_A_TEXTURE);
    
    glInit();
    glEnable(GL_TEXTURE_2D);
    glClearColor(222, 245, 255, 31);
    glViewport(0, 0, 255, 191);
	
    LoadGLTextures();
	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 256.0 / 192.0, 0.1, 40);	
    gluLookAt(0.0, 0.0, 1.0,  //camera possition 
              0.0, 0.0, 0.0,  //look at
              0.0, 1.0, 0.0); //up

    float rotateY = 0.0;

    while(1) {
        glPushMatrix();
        glTranslatef32(0, 0, floattof32(-2));
        glRotateY(rotateY);
		
        glMatrixMode(GL_MODELVIEW);
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);
        glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, texture[0]);

        for(int i = 0; i < model->count; i++) {
            struct mesh *curr = &model->meshes[i];

            for(int j = 0; j < curr->count; j++) {
                glBegin(GL_TRIANGLE);
			        glTexCoord2f(curr->uvs[j * 3].x, curr->uvs[j * 3].y);
                    glVertex3v16(
	                    floattof32(curr->vertices[curr->indices[j * 3]].x),
		                floattof32(curr->vertices[curr->indices[j * 3]].y),
	                    floattof32(curr->vertices[curr->indices[j * 3]].z)
                    );

                    glTexCoord2f(curr->uvs[j * 3 + 1].x, curr->uvs[j * 3 + 1].y);
                    glVertex3v16(
		                floattof32(curr->vertices[curr->indices[j * 3 + 1]].x),
	                    floattof32(curr->vertices[curr->indices[j * 3 + 1]].y),
                        floattof32(curr->vertices[curr->indices[j * 3 + 1]].z)
	                );

                    glTexCoord2f(curr->uvs[j * 3 + 2].x, curr->uvs[j * 3 + 2].y);
                    glVertex3v16(
	                    floattof32(curr->vertices[curr->indices[j * 3 + 2]].x),
		                floattof32(curr->vertices[curr->indices[j * 3 + 2]].y),
	                    floattof32(curr->vertices[curr->indices[j * 3 + 2]].z)
	                );
                glEnd();
            }
        }
		
        glPopMatrix(1);
        glFlush(0);
        rotateY -= 1.5;
		
        swiWaitForVBlank();
    }
	
    free(model);

    return 0;
}

