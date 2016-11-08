#include <nds.h>
#include <filesystem.h>
#include <stdio.h>
#include "model.h"

int main(void) {
	consoleDemoInit();

	if (!nitroFSInit(NULL)) {
		iprintf("Error could not initialize file system.\n");
		return 0;
	}
	
	struct model *ico = load_model("ico.dae");
	
	videoSetMode(MODE_0_3D);
	glInit();
	glClearColor(0, 0, 0, 31);
	glViewport(0, 0, 255, 191);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 256.0 / 192.0, 0.1, 40);
	
	gluLookAt(0.0, 0.0, 1.0,  //camera possition 
			  0.0, 0.0, 0.0,  //look at
			  0.0, 1.0, 0.0); //up

	float rotateX = 0.0;
	float rotateY = 0.0;

	while(1) {
		glPushMatrix();
		glTranslatef32(0, 0, floattof32(-1));
		glRotateX(rotateX);
		glRotateY(rotateY);
		glMatrixMode(GL_MODELVIEW);
		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);
		
		for(int i = 0; i < ico->count; i += 3) {
			glBegin(GL_TRIANGLE);
				glColor3b(255,0,0);
				glVertex3v16(
					floattof32(ico->vertices[ico->indices[i] * 3]),
					floattof32(ico->vertices[ico->indices[i] * 3 + 1]),
					floattof32(ico->vertices[ico->indices[i] * 3 + 2])
				);

				glColor3b(0,255,0);
				glVertex3v16(
					floattof32(ico->vertices[ico->indices[i + 1] * 3]),
					floattof32(ico->vertices[ico->indices[i + 1] * 3 + 1]),
					floattof32(ico->vertices[ico->indices[i + 1] * 3 + 2])
				);

				glColor3b(0,0,255);
				glVertex3v16(
					floattof32(ico->vertices[ico->indices[i + 2] * 3]),
					floattof32(ico->vertices[ico->indices[i + 2] * 3 + 1]),
					floattof32(ico->vertices[ico->indices[i + 2] * 3 + 2])
				);
			glEnd();
		}
		
		glPopMatrix(1);
		glFlush(0);
		
		rotateX += 2;
		rotateY -= 1;
		
		swiWaitForVBlank();
	}
	
	free(ico);

	return 0;
}

