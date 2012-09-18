/*
 *	OBJModelViewer.cc
 * 
 *	A OBJ model viewer.
 *
 *	Coded by Joseph A. Marrero
 *	1/15/2007
 */
#include "OBJModelViewer.h"
#include <cassert>
#include <iostream>
#include "OBJLoader/OBJLoader.h"

using namespace std;

int main( int argc, char *argv[] )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( 640, 480 );
	
	mainWindow = glutCreateWindow( "OBJ Model Viewer" );
#ifndef _DEBUG
	glutFullScreen( );
#endif
	glutDisplayFunc( render );
	GLUI_Master.set_glutReshapeFunc( resize );
	GLUI_Master.set_glutKeyboardFunc( keyboard_keypress );
	GLUI_Master.set_glutMouseFunc( mouse );
	glutMotionFunc( mouseMotion );
	GLUI_Master.set_glutIdleFunc( idle );



	initialize( );
	glutMainLoop( );
	deinitialize( );

	return 0;
}


void initialize( )
{
	glEnable( GL_ALPHA_TEST );
	
	glEnable( GL_POINT_SMOOTH );
	glEnable( GL_LINE_SMOOTH );
	//glEnable( GL_POLYGON_SMOOTH );
	glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	//glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	

	glShadeModel( GL_SMOOTH );
	glClearDepth( 1.0f );		
	glDepthFunc( GL_LEQUAL );
	glEnable( GL_DEPTH_TEST );


	glPixelStorei( GL_PACK_ALIGNMENT, 4 );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );


	glEnable( GL_LIGHTING );
	glLightfv( GL_LIGHT0, GL_POSITION, light1Position );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, light1Diffuse );
	glLightfv( GL_LIGHT0, GL_AMBIENT, light1Ambient );
	glLightfv( GL_LIGHT0, GL_SPECULAR, light1Specular );
	glEnable( GL_LIGHT0 );

	glLightfv( GL_LIGHT1, GL_POSITION, light2Position );
	glLightfv( GL_LIGHT1, GL_SPOT_DIRECTION, light2Direction );
	glLightf( GL_LIGHT1, GL_SPOT_CUTOFF, 85.0f );
	glLightfv( GL_LIGHT1, GL_DIFFUSE, light2Diffuse );
	glLightfv( GL_LIGHT1, GL_AMBIENT, light2Ambient );
	glLightfv( GL_LIGHT1, GL_SPECULAR, light2Specular );
	

	glLightModelf( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE );
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, light1Ambient );
	// TO DO: Initialization code goes here...
	for( unsigned int i = 0; i < 4; i++ )
	{
		ViewingTransform[ i ].zoom = 15.0f;
		ViewingTransform[ i ].rotx = 0;
		ViewingTransform[ i ].roty = 0.001f;
		ViewingTransform[ i ].tx = 0;
		ViewingTransform[ i ].ty = 0;
	}


	Mouse.lastx = 0;
	Mouse.lasty = 0;
	currentViewport = VP_PERSPECTIVE;
	memset( &Mouse.buttons[0], 0, sizeof(unsigned char) * 3 );



	// Initialize PUI components...
	int width = glutGet( (GLenum) GLUT_WINDOW_WIDTH );
	int height = glutGet( (GLenum) GLUT_WINDOW_HEIGHT );
	pChooseModelWindow = GLUI_Master.create_glui_subwindow( mainWindow, GLUI_SUBWINDOW );
	pFileBrowser = new GLUI_FileBrowser( pChooseModelWindow, "Choose a Model File", GLUI_PANEL_EMBOSSED, FILE_BROWSER_ID, loadModel );
	pFileBrowser->set_w( width >> 1 );
	pFileBrowser->set_h( height >> 1 );

	new GLUI_Button( pChooseModelWindow, "Cancel", BTN_CANCEL_ID, loadModel );
	pChooseModelWindow->hide( );
	pChooseModelWindow->set_main_gfx_window( mainWindow );




	pGLUI = GLUI_Master.create_glui_subwindow( mainWindow, GLUI_SUBWINDOW_TOP );
	pGLUI->bkgd_color_f[ 0 ] = 0.4f;
	pGLUI->bkgd_color_f[ 1 ] = 0.5f;
	pGLUI->bkgd_color_f[ 2 ] = 0.6f;
	pLoadModelBtn = new GLUI_Button( pGLUI, "Load Model", LOAD_MODEL_ID, OnFileOpen );
	new GLUI_Column( pGLUI, false );
	pSwitchViewBtn = new GLUI_Button( pGLUI, "Perspective", SWITCH_VIEW_ID, OnViewButton );
	new GLUI_Column( pGLUI, false );
	new GLUI_Button( pGLUI, "Exit", EXIT_ID, OnExit );
	new GLUI_Column( pGLUI, true );
	pTitle = new GLUI_StaticText( pGLUI, "Untitled" );
	pTitle->set_font( GLUT_BITMAP_9_BY_15 );
	pGLUI->set_main_gfx_window( mainWindow );
}

void deinitialize( )
{
	pChooseModelWindow->close( );
	pGLUI->close( );
}


void render( )
{
	glClearColor( 0.4f, 0.4f, 0.4f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity( );
	
	int width = glutGet( (GLenum) GLUT_WINDOW_WIDTH );
	int height = glutGet( (GLenum) GLUT_WINDOW_HEIGHT );

	switch( currentViewSetting )
	{
		case V_FOURVIEW:
			render4View( );
			break;
		case V_ONEVIEW:
			render1View();
			break;
		default:
			break;
	}

	glutSwapBuffers( );
}

void resize( int width, int height )
{
	if( height == 0 )
		height = 1;
	
	aspect = (double) width / (double) height;
}

void keyboard_keypress( unsigned char key, int x, int y )
{
	//puKeyboard( key, PU_DOWN );

	//switch( key )
	//{
	//	case 'Q':
	//	case 'q':
	//	case GLUT_KEY_F1:
	//	case GLUT_KEY_END:
	//		deinitialize( );
	//		exit( 0 );
	//		break;
	//	default:
	//		break;
	//}

}

void mouse( int button, int state, int x, int y )
{
	int puRet = 0;
	Mouse.lastx = x;
	Mouse.lasty = y;

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			Mouse.buttons[ 0 ] = ( (GLUT_DOWN == state) ? 1 : 0 );


			if( !puRet && currentViewSetting == V_FOURVIEW )
			{
				if( state = 1 && testClickInViewport( VP_PERSPECTIVE, x, y ) )
					currentViewport = VP_PERSPECTIVE;
				else if( state = 1 && testClickInViewport( VP_SIDE, x, y ) )
					currentViewport = VP_SIDE;
				else if( state = 1 && testClickInViewport( VP_FRONT, x, y ) )
					currentViewport = VP_FRONT;
				else if( state = 1 && testClickInViewport( VP_TOP, x, y ) )
					currentViewport = VP_TOP;
				else 
					currentViewport = VP_NONE;
			}


			break;
		case GLUT_MIDDLE_BUTTON:
			Mouse.buttons[ 1 ] = ( (GLUT_DOWN == state) ? 1 : 0 );
			break;
		case GLUT_RIGHT_BUTTON:
			Mouse.buttons[ 2 ] = ( (GLUT_DOWN == state) ? 1 : 0 );
			break;
		default:
			break;		
	}

	glutPostRedisplay( );
}

void mouseMotion( int x, int y )
{
	int diffx = x - Mouse.lastx;
	int diffy = y - Mouse.lasty;
	Mouse.lastx = x;
	Mouse.lasty = y;


	if( Mouse.buttons[ 0 ] )
	{
		ViewingTransform[ currentViewport ].rotx += (float) 0.5f * diffy;
		ViewingTransform[ currentViewport ].roty += (float) 0.5f * diffx;
	}
	else if( Mouse.buttons[ 1 ] )
	{
		ViewingTransform[ currentViewport ].tx += (float) 0.001f  * diffx * (abs(ViewingTransform[ currentViewport ].zoom) + 1);
		ViewingTransform[ currentViewport ].ty -= (float) 0.001f * diffy * (abs(ViewingTransform[ currentViewport ].zoom) + 1);
	}
	else if( Mouse.buttons[ 2 ] )
	{
		ViewingTransform[ currentViewport ].zoom -= (float) 0.05f * (abs(ViewingTransform[ currentViewport ].zoom) + 1) * diffy;
		if( ViewingTransform[ currentViewport ].zoom <= 0.0f )
			ViewingTransform[ currentViewport ].zoom = 0.0f;
	}

	glutPostRedisplay( );
}

void idle( )
{ glutPostRedisplay( ); }

void writeText( void *font, std::string &text, int x, int y )
{
	int width = glutGet( (GLenum) GLUT_WINDOW_WIDTH );
	int height = glutGet( (GLenum) GLUT_WINDOW_HEIGHT );

	glDisable( GL_DEPTH_TEST );
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );

	glMatrixMode( GL_PROJECTION );
	glPushMatrix( );
		glLoadIdentity( );	
		glOrtho( 0, width, 0, height, 1.0, 5.0 );
			
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix( );
			glLoadIdentity( );
			glColor3f( 1.0f, 1.0f, 1.0f );
			glTranslatef( 0.0f, 0.0f, -1.0f );
			glRasterPos2i( x, y );

			for( unsigned int i = 0; i < text.size( ); i++ )
				glutBitmapCharacter( font, text[ i ] );
			
		glPopMatrix( );
		glMatrixMode( GL_PROJECTION );
	glPopMatrix( );
	glMatrixMode( GL_MODELVIEW );

	glEnable( GL_LIGHTING );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_TEXTURE_2D );

}

void viewingTransform( Viewport v )
{	
	if( v == VP_PERSPECTIVE )
	{
		glTranslatef( 0.0f, 0.0f, -ViewingTransform[ v ].zoom );
		glTranslatef( ViewingTransform[ v ].tx, ViewingTransform[ v ].ty, 0.0f );
		glRotatef( ViewingTransform[ v ].rotx, 1.0f, 0.0f, 0.0f );
		glRotatef( ViewingTransform[ v ].roty, 0.0f, 1.0f, 0.0f );	
	}
	else {
		glScalef( 1.0f / ViewingTransform[ v ].zoom, 1.0f / ViewingTransform[ v ].zoom, 1.0f / ViewingTransform[ v ].zoom );
		glTranslatef( ViewingTransform[ v ].tx, ViewingTransform[ v ].ty, 0.0f );
	}

}

bool testClickInViewport( Viewport v, int x, int y )
{
	int width = glutGet( (GLenum) GLUT_WINDOW_WIDTH );
	int height = glutGet( (GLenum) GLUT_WINDOW_HEIGHT );

	switch( v )
	{
		case VP_PERSPECTIVE:
			if( (x > width / 2.0) && (height - y > height / 2.0) ) return true;
			break;
		case VP_FRONT:
			if( (x < width / 2.0) && (height - y < height / 2.0) ) return true;
			break;
		case VP_SIDE:
			if( (x < width / 2.0) && (height - y > height / 2.0) ) return true;
			break;
		case VP_TOP:
			if( (x > width / 2.0) && (height - y < height / 2.0) ) return true;
			break;
		default:
			return false;
	}

	return false;
}

void drawGrid( )
{
	// draw grid
	glDisable( GL_LIGHTING );
	glBegin( GL_LINES);
	glColor4f( 0.8f, 0.8f, 0.8f, 0.5f );
	for( int i = -10; i <= 10; ++i )
	{
		glVertex3f( i, 0, -10 );
		glVertex3f( i, 0, 10 );

		glVertex3f( 10, 0, i );
		glVertex3f( -10, 0, i );
	}
	glEnd( );
	glEnable( GL_LIGHTING );
}

void drawModel( )
{
	if( meshData.size() > 0 )
	{
		glColor3f( 1.0f, 1.0f, 1.0f );
		glInterleavedArrays( GL_T2F_N3F_V3F, 0, &meshData[ 0 ] );
		glDrawArrays( GL_TRIANGLES, 0, meshData.size( ) );
	}
}


void OnFileOpen( int control )
{
	pChooseModelWindow->show();	
}


void OnExit( int control )
{
	deinitialize( );
	exit( 0 ); 
}

void loadModel( int control )
{
	if( control == FILE_BROWSER_ID ) 
	{
		const char *filename = pFileBrowser->get_file( );
		printf( "%s\n", filename );
		OBJLoader loader;
		loader.load( filename, true );

		OBJLoader::GroupCollection &objMesh = loader.mesh( );

		// looks odd, but is neccessary since we must combine any seperate
		// meshes into one mesh. The separate meshes are described by groups.
		meshData.clear( );
		for( unsigned int i = 0; i < objMesh.size(); i++ )
			for( unsigned int j = 0; j < objMesh[ i ].mesh.size(); j++ )
				meshData.push_back( objMesh[ i ].mesh[ j ] );

		pTitle->set_text( filename );
		loader.clear( );
	}

	pChooseModelWindow->hide();	
}

void render4View( )
{
	int width = glutGet( (GLenum) GLUT_WINDOW_WIDTH );
	int height = glutGet( (GLenum) GLUT_WINDOW_HEIGHT );

	// Front
	glViewport( 0, 0, width / 2.0, height / 2.0 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	glOrtho( -1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -500.0, 500.0 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	viewingTransform( VP_FRONT );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	renderCoordinateAxes( );
	drawGrid( );
	drawModel( );
	writeText( GLUT_BITMAP_HELVETICA_12, std::string("Front"), 5, 12 );

	// Top
	glViewport( width / 2.0, 0, width / 2.0, height / 2.0 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	glOrtho( -1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -500.0, 500.0 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	viewingTransform( VP_TOP );
	glRotatef( 90.0f, 1.0, 0.0f, 0.0f );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	renderCoordinateAxes( );
	drawGrid( );
	drawModel( );
	writeText( GLUT_BITMAP_HELVETICA_12, std::string("Top"), 5, 12 );

	// Side
	glViewport( 0, height / 2.0, width / 2.0, height / 2.0 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	glOrtho( -1.0 * aspect, 1.0 * aspect, -1.0, 1.0, -500.0, 500.0 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	viewingTransform( VP_SIDE );
	glRotatef( 90.0f, 0.0, 1.0f, 0.0f );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	renderCoordinateAxes( );
	drawGrid( );
	drawModel( );
	writeText( GLUT_BITMAP_HELVETICA_12, std::string("Side"), 5, 12 );


	// Perspective
	glViewport( width / 2.0, height / 2.0, width / 2.0, height / 2.0 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluPerspective( 40.0f, aspect, 0.1, 500.0 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glLightfv( GL_LIGHT1, GL_POSITION, light2Position );
	glLightfv( GL_LIGHT1, GL_SPOT_DIRECTION, light2Direction );
	
	viewingTransform( VP_PERSPECTIVE );

	renderCoordinateAxes( );
	drawGrid( );
	glEnable( GL_LIGHT1 );
	drawModel( );
	glDisable( GL_LIGHT1 );
	writeText( GLUT_BITMAP_HELVETICA_12, std::string("Perspective"), 5, 12 );


	/* Seperator lines */
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	glOrtho( 0, width, 0, height, -5.0, 5.0 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef( 0, 0, -3 );

	glColor3f( 0, 0, 0 );
	//glLineWidth( 2.0f );
	glBegin( GL_LINES );
	glVertex2i( width >> 1, 0 );
	glVertex2i( width >> 1, height >> 1 );

	glVertex2i( 0, height >> 1 );
	glVertex2i( width >> 1, height >> 1 );


	glVertex2i( width >> 1, height );
	glVertex2i( width >> 1, height >> 1 );

	glVertex2i( width >> 1, height >> 1 );
	glVertex2i( width, height >> 1 );
	glEnd( );
	//glLineWidth( 1.0f );
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

void render1View( )
{
	int width = glutGet( (GLenum) GLUT_WINDOW_WIDTH );
	int height = glutGet( (GLenum) GLUT_WINDOW_HEIGHT );
	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluPerspective( 40.0f, aspect, 0.1, 500.0 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glLightfv( GL_LIGHT1, GL_POSITION, light2Position );
	glLightfv( GL_LIGHT1, GL_SPOT_DIRECTION, light2Direction );

	viewingTransform( VP_PERSPECTIVE );

	renderCoordinateAxes( );
	drawGrid( );
	glEnable( GL_LIGHT1 );
	drawModel( );
	glDisable( GL_LIGHT1 );
	writeText( GLUT_BITMAP_HELVETICA_12, std::string("Perspective"), 5, 12 );
}

void renderCoordinateAxes( )
{
	glPushMatrix( );
	glDisable( GL_BLEND );
	glDisable( GL_LIGHTING );

	glBegin( GL_LINES );
	// x
	glColor3f( 1.0f, 0.0f, 0.0f );
	glVertex3i( 0, 0, 0);
	glVertex3i( 1, 0, 0);
	// y
	glColor3f( 0.0f, 1.0f, 0.0f );
	glVertex3i( 0, 0, 0);
	glVertex3i( 0, 1, 0);
	// z
	glColor3f( 0.0f, 0.0f, 1.0f );
	glVertex3i( 0, 0, 0);
	glVertex3i( 0, 0, 1);
	glEnd( );

	glEnable( GL_BLEND );
	glEnable( GL_LIGHTING );
	glPopMatrix( );
}

void OnViewButton( int control )
{
	switch( currentViewSetting )
	{
	case V_FOURVIEW:
		currentViewSetting = V_ONEVIEW;
		pSwitchViewBtn->set_name( "4 - Views" );
		currentViewport = VP_PERSPECTIVE;
		break;
	case V_ONEVIEW:
		currentViewSetting = V_FOURVIEW;
		pSwitchViewBtn->set_name( "Perspective" );
		break;
	default:
		break;
	}
	
	glutPostRedisplay( );
}
