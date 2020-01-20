#include <nds.h>
#include <stdlib.h>

#include <math.h>

//texture_bin.h is created automagicaly from the texture.bin placed in arm9/resources
//texture.bin is a raw 128x128 16 bit image.  I will release a tool for texture conversion 
//later
#include "texture.h"


//verticies for the cube
v16 CubeVectors[] = {
 		floattov16(-0.5), floattov16(-0.5), floattov16(0.5), 
		floattov16(0.5),  floattov16(-0.5), floattov16(0.5),
		floattov16(0.5),  floattov16(-0.5), floattov16(-0.5),
		floattov16(-0.5), floattov16(-0.5), floattov16(-0.5),
		floattov16(-0.5), floattov16(0.5),  floattov16(0.5), 
		floattov16(0.5),  floattov16(0.5),	floattov16(0.5),
		floattov16(0.5),  floattov16(0.5),  floattov16(-0.5),
		floattov16(-0.5), floattov16(0.5),  floattov16(-0.5)
};

struct Vector3 {
	float x, y, z;
};

struct Vector3 cubePositions[100];
struct Vector3 cubeRotations[100];

//polys
u8 CubeFaces[] = {
	3,2,1,0,
	0,1,5,4,
	1,2,6,5,
	2,3,7,6,
	3,0,4,7,
	5,6,7,4
};

//texture coordinates
u32 uv[] =
{
	
	TEXTURE_PACK(inttot16(128), 0),
	TEXTURE_PACK(inttot16(128),inttot16(128)),
	TEXTURE_PACK(0, inttot16(128)),
	TEXTURE_PACK(0,0)
};

u32 normals[] =
{
	NORMAL_PACK(0,floattov10(-.97),0),
	NORMAL_PACK(0,0,floattov10(.97)),
	NORMAL_PACK(floattov10(.97),0,0),
	NORMAL_PACK(0,0,floattov10(-.97)),
	NORMAL_PACK(floattov10(-.97),0,0),
	NORMAL_PACK(0,floattov10(.97),0)
	
};

//draw a cube face at the specified color
 void drawQuad(int poly)
{	
	
	u32 f1 = CubeFaces[poly * 4] ;
	u32 f2 = CubeFaces[poly * 4 + 1] ;
	u32 f3 = CubeFaces[poly * 4 + 2] ;
	u32 f4 = CubeFaces[poly * 4 + 3] ;


	glNormal(normals[poly]);

	GFX_TEX_COORD = (uv[0]);
	glVertex3v16(CubeVectors[f1*3], CubeVectors[f1*3 + 1], CubeVectors[f1*3 +  2] );
	
	GFX_TEX_COORD = (uv[1]);
	glVertex3v16(CubeVectors[f2*3], CubeVectors[f2*3 + 1], CubeVectors[f2*3 + 2] );
	
	GFX_TEX_COORD = (uv[2]);
	glVertex3v16(CubeVectors[f3*3], CubeVectors[f3*3 + 1], CubeVectors[f3*3 + 2] );

	GFX_TEX_COORD = (uv[3]);
	glVertex3v16(CubeVectors[f4*3], CubeVectors[f4*3 + 1], CubeVectors[f4*3 + 2] );
}

int main() {	
	
	int textureID;
	int i, j;
	struct Vector3 camPos;
	float camRot;
	struct Vector3 camFace;
	struct Vector3 camLook;

	//set mode 0, enable BG0 and set it to 3D
	videoSetMode(MODE_0_3D);

	// initialize gl
	glInit();
	
	//enable textures
	glEnable(GL_TEXTURE_2D);
	
	//this should work the same as the normal gl call
	glViewport(0,0,255,191);
	
	// enable antialiasing
	glEnable(GL_ANTIALIAS);
	
	// setup the rear plane
	glClearColor(0,0,0,31); // BG must be opaque for AA to work
	glClearPolyID(63); // BG must have a unique polygon ID for AA to work
	glClearDepth(0x7FFF);

	vramSetBankA(VRAM_A_TEXTURE);

	glGenTextures(1, &textureID);
	glBindTexture(0, textureID);
	glTexImage2D(0, 0, GL_RGB, TEXTURE_SIZE_128 , TEXTURE_SIZE_128, 0, TEXGEN_TEXCOORD, (u8*)textureBitmap);
	
	
	//any floating point gl call is being converted to fixed prior to being implemented
	
	for(i = 0; i < 100; i++){
		cubePositions[i].x = (float)(250 - (rand() % 500));
		cubePositions[i].y = (float)(250 - (rand() % 500));
		cubePositions[i].z = (float)(250 - (rand() % 500));
		cubeRotations[i].x = (float)(rand() % 360);
		cubeRotations[i].y = (float)(rand() % 360);
		cubeRotations[i].z = (float)(rand() % 360);
	}
	
	camPos.x = 0.0f;
	camPos.y = 0.0f;
	camPos.z = 0.0f;
	camFace.x = 0.0f;
	camFace.y = 0.0f;
	camFace.z = 0.0f;
	camRot = 0.0f;
	
	while(1) {
		
		glLight(0, RGB15(31,0,0) , 0,				  floattov10(-1.0),		 0);
		glLight(1, RGB15(0,31,0),   0,				  floattov10(1) - 1,			 0);
		glLight(2, RGB15(0,0,31) ,   floattov10(-1.0), 0,					 0);
		glLight(3, RGB15(31,31,31) ,   floattov10(1.0) - 1,  0,					 0);

		glMaterialf(GL_AMBIENT, RGB15(8,8,8));
		glMaterialf(GL_DIFFUSE, RGB15(16,16,16));
		glMaterialf(GL_SPECULAR, BIT(15) | RGB15(8,8,8));
		glMaterialf(GL_EMISSION, RGB15(5,5,5));

		//ds uses a table for shinyness..this generates a half-ass one
		glMaterialShinyness();

		//not a real gl function and will likely change
		glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 | POLY_FORMAT_LIGHT1 | 
													POLY_FORMAT_LIGHT2 | POLY_FORMAT_LIGHT3 ) ;
		
		scanKeys();
		
		u16 keys = keysHeld();
		
		if(keys & KEY_A) camPos.y += 0.5f;
		if(keys & KEY_B) camPos.y -= 0.5f;
		
		if(keys & KEY_LEFT) camRot += 0.05f;
		if(keys & KEY_RIGHT) camRot -= 0.05f;
		
		if(keys & KEY_UP){
			camPos.x += camFace.x;
			camPos.z += camFace.z;
		}
		
		if(keys & KEY_DOWN){
			camPos.x -= camFace.x;
			camPos.z -= camFace.z;
		}
		
		camFace.x = sin(camRot);
		camFace.y = camPos.y;
		camFace.z = cos(camRot);
		camLook.x = camFace.x + camPos.x;
		camLook.y = camFace.y;
		camLook.z = camFace.z + camPos.z;
		
		glBindTexture(0, textureID);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(70, 256.0 / 192.0, 0.1, 10000);

		//draw the obj
		glBegin(GL_QUAD);
		for(i = 0; i < 100; i++){
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			
			gluLookAt(camPos.x, camPos.y, camPos.z,		//camera possition 
					camLook.x, camLook.y, camLook.z,		//look at
					0.0, 1.0, 0.0);		//up
			
			//move it away from the camera
			glTranslatef(cubePositions[i].x, cubePositions[i].y, cubePositions[i].z);
					
			glRotateX(cubeRotations[i].x);
			glRotateY(cubeRotations[i].y);
			glRotateZ(cubeRotations[i].z);
			
			glScalef(20.0f, 20.0f, 20.0f);
			
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
		
			for(j = 0; j < 6; j++){
				drawQuad(j);
			}
		}
		
		glEnd();
			
		glFlush(0);

		swiWaitForVBlank();

		if(keys & KEY_START) break;
	}

	return 0;
}//end main 
