#include <nds.h>
#include <filesystem.h>
#include <stdio.h>
#include <nds/arm9/image.h>
#include <sys/stat.h>
#include "model.h"

int texture[2];

int LoadGLTextures() {
    sImage pcx;
    struct stat sb;
    unsigned char *contents;
    
    if(stat("pipe.pcx", &sb) != 0) {
        iprintf("Failed to get stat.\n");
        return FALSE;
    }

    FILE *file = fopen("pipe.pcx", "rb");
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
    free(contents);
    fclose(file);
    
    if(stat("goomba.pcx", &sb) != 0) {
        iprintf("Failed to get stat.\n");
        return FALSE;
    }

    file = fopen("goomba.pcx", "rb");
    contents = (unsigned char *)malloc(sb.st_size + 1);
    fread(contents, sizeof(unsigned char), sb.st_size, file);

    if(!file) {
        iprintf("Failed to open file.\n");
        return FALSE;   
    }

    loadPCX((u8*)contents, &pcx);	
    image8to16(&pcx);

    glGenTextures(1, &texture[1]);
    glBindTexture(0, texture[1]);
    glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_128, TEXTURE_SIZE_128, 0,
        TEXGEN_TEXCOORD, pcx.image.data8);

    imageDestroy(&pcx);
    free(contents);
    fclose(file);

    return TRUE;
}

int main(void) {
    consoleDemoInit();

    if (!nitroFSInit(NULL)) {
        iprintf("Error could not initialize file system.\n");
        return 0;
    }
	
    struct model *pipe = load_model("pipe.sos");
    struct model *goomba = load_model("goomba.sos");
	
    videoSetMode(MODE_0_3D);
    vramSetBankA(VRAM_A_TEXTURE);
    
    glInit();
    glEnable(GL_TEXTURE_2D);
    glClearColor(249, 250, 255, 31);
    glViewport(0, 0, 255, 191);
	
    LoadGLTextures();
	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 256.0 / 192.0, 0.1, 40);	
    gluLookAt(0.0, 0.0, 1.0,  //camera position 
              0.0, 0.0, 0.0,  //look at
              0.0, 1.0, 0.0); //up
              
    float goomba_y = -2.0;

    while(1) {
        glPushMatrix();
        glTranslatef32(0, floattof32(-1.2), floattof32(-1.5));
        glRotateX(12);
		glRotateY(-65);
		
        glMatrixMode(GL_MODELVIEW);
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);
        glColor3f(1, 1, 1);
        
		glBindTexture(GL_TEXTURE_2D, texture[0]);
        glPushMatrix();
        for(int i = 0; i < pipe->count; i++) {
            struct mesh *curr = &pipe->meshes[i];

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
        
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        glPushMatrix();
        glTranslatef32(0, floattof32(goomba_y), 0);
        glRotateY(65);
        for(int i = 0; i < goomba->count; i++) {
            struct mesh *curr = &goomba->meshes[i];

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
		
        glPopMatrix(1);
        glFlush(0);
        
        // loop the goomba :)
        if(goomba_y < 2.5) {
            goomba_y += 0.02;
        } else {
            goomba_y = -2.0;
        }
		
        swiWaitForVBlank();
    }
	
    free(pipe);
    free(goomba);

    return 0;
}

