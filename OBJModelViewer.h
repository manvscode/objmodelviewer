#ifndef _OBJMODELVIEWER_H_
#define _OBJMODELVIEWER_H_
/*
 *	OBJModelViewer.h
 *
 *	A OBJ model viewer.
 *
 *	Coded by Joseph A. Marrero
 *	1/15/2007
 */
#include <GL/glew.h>
#include <GL/glui.h>
#include <string>
#include <vector>
#include "OBJLoader/OBJLoader.h"

using namespace std;


int mainWindow = 0;
GLUI *pGLUI = NULL;
GLUI *pChooseModelWindow = NULL;
GLUI_Button *pLoadModelBtn = NULL;
GLUI_Button *pSwitchViewBtn = NULL;
GLUI_FileBrowser *pFileBrowser = NULL;
GLUI_StaticText *pTitle = NULL;
#define LOAD_MODEL_ID		200
#define SWITCH_VIEW_ID		201
#define EXIT_ID				202
//#define BTN_LOAD_MODEL_ID	203
#define BTN_CANCEL_ID		204
#define FILE_BROWSER_ID		205
void initialize( );
void deinitialize( );

void render( );
void resize( int width, int height );
void keyboard_keypress( unsigned char key, int x, int y );
void mouse( int button, int state, int x, int y );
void mouseMotion( int x, int y );
void idle( );
void writeText( void *font, std::string &text, int x, int y );

struct tagMouse {
	int lastx;
	int lasty;
	unsigned char buttons[ 3 ];
} Mouse;

typedef enum tagViewport {
	VP_PERSPECTIVE,
	VP_FRONT,
	VP_TOP,
	VP_SIDE,
	VP_NONE
} Viewport;

struct tagViewingTransforms {
	float zoom;
	float rotx;
	float roty;
	float tx;
	float ty;
} ViewingTransform[ 4 ];

double aspect;
Viewport currentViewport = VP_NONE;

float light1Position[] = { 0.0f, 0.0f, 0.0f, 1.0f }; // 4th component is 1 so that its infinitely away.
float light1Diffuse[] = { 0.8f, 0.8f, 0.8f, 0.0f };
float light1Ambient[] = { 0.15f, 0.15f, 0.15f, 0.0f };
float light1Specular[] = { 0.8f, 0.8f, 0.8f, 0.0f };

float light2Position[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float light2Direction[] = { 0.0f, 0.0f, -1.0f, 0.0f };
float light2Diffuse[] = { 0.35f, 0.35f, 0.35f, 0.0f };
float light2Ambient[] = { 0.02f, 0.02f, 0.02f, 0.0f };
float light2Specular[] = { 0.4f, 0.4f, 0.4f, 0.0f };

void viewingTransform( Viewport v );
bool testClickInViewport( Viewport v, int x, int y );
void drawGrid( );
void drawModel( );



/*
 *	UI Stuff
 */
void OnFileOpen( int control );
void OnExit( int control );
void OnViewButton( int control );


typedef enum tagView {
	V_FOURVIEW,
	V_ONEVIEW
}View;

View currentViewSetting = V_FOURVIEW;

vector<OBJLoader::MeshVertex> meshData;

void loadModel( int control );

void render4View( );
void render1View( );
void renderCoordinateAxes( );

#endif
