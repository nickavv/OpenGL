/**
   @file terrain.cpp
   @authors John Huston, Nicholas StPierre, Chris Compton
   @date 2012-12-06
   @brief This is a derivative of our main project file, fly.cpp.
   @details This is a tech demo for terrain generation using an udpated
   engine derived from fly.cpp, which was mostly based on Ed Angel's code
   from his book.
**/

#include "globals.h"
/* System Headers */
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <time.h>
/* Multi-platform support and OpenGL headers. */
#include "platform.h"
/* Ed Angel's Math Classes */
#include "vec.hpp"
#include "mat.hpp"
/* Utilities and Classes */
#include "model.hpp"
#include "Camera.hpp"
#include "InitShader.hpp"
#include "Cameras.hpp"
#include "Screen.hpp"
#include "Object.hpp"
#include "Timer.hpp"
#include "Scene.hpp"

// Type Aliases
using Angel::vec3;
using Angel::vec4;
typedef Angel::vec4 color4;
typedef Angel::vec4 point4;

// Wii Connectivity 
#ifdef WII
#include <wiicpp.h>  // WiiC++ API
#include "WiiUtil.h" // Wii Controller Handler Util
CWii Wii;
bool usingWii = false;
#endif
////

Screen myScreen( 800, 600 );
Scene theScene;
GLuint gShader;
bool fixed_yaw = true;

// Textures
// Obtained from www.goodtextures.com
const char* terrainTex[] = {
  "../Textures/GrassGreenTexture0002.jpg", // Grass (who'da thunk?)
  "../Textures/GoodTextures_0013418.jpg",  // Rock
  "../Textures/GoodTextures_0013291.jpg"   // Snow
};

void cameraInit( Camera& cam ) {

  /* Link this camera to our standard shader variables. */
  cam.link( gShader, Camera::TRANSLATION, "T" );
  cam.link( gShader, Camera::ROTATION, "R" );
  cam.link( gShader, Camera::VIEW, "P" );
  cam.link( gShader, Camera::CTM, "CTM" );

}

void init() {

  srand( time(NULL));
  // Load shaders and use the resulting shader program
  gShader = Angel::InitShader( "shaders/vterrain.glsl", "shaders/fterrain.glsl" );
  theScene.SetShader( gShader );

  Object *terrain = theScene.AddObject( "terrain" );
  Object *pyramid = theScene.AddObject( "pyramid" );
  Object *cube = pyramid->AddObject( "colorcube" );

  /** Fill points[...] with terrain map **/
  landGen( terrain, 8, 40.0 );
  terrain->Texture( terrainTex );
  terrain->Buffer();
  terrain->Mode( GL_TRIANGLE_STRIP );

  Sierpinski_Pyramid( pyramid,
		      vec4(  0,      1,  0, 1 ),
		      vec4( -1, -0.999,  1, 1 ),
		      vec4(  1, -0.999,  1, 1 ),
		      vec4(  0, -0.999, -1, 1 ),
		      4 );
  pyramid->Buffer();
  pyramid->Mode( GL_TRIANGLES );

  colorcube( cube, 2.0 );
  cube->Buffer();
  cube->Mode( GL_LINE_LOOP );
    
  // Link however many cameras we have at this point to the shader.
  myScreen.camList.LinkAll( gShader, Camera::TRANSLATION, "T" );
  myScreen.camList.LinkAll( gShader, Camera::ROTATION, "R" );
  myScreen.camList.LinkAll( gShader, Camera::VIEW, "P" );
  myScreen.camList.LinkAll( gShader, Camera::CTM, "CTM" );

  glEnable( GL_DEPTH_TEST );
  glClearColor( 0.5, 0.7, 1.0, 1.0 );
  
}

//--------------------------------------------------------------------

/** A function that takes no arguments.
    Is responsible for drawing a SINGLE VIEWPORT. **/
void displayViewport( void ) {  
  theScene.Draw();
}

void display( void ) {
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // Tell camList to draw using our displayViewport rendering function.
  myScreen.camList.Draw( displayViewport );
  glutSwapBuffers();
}

void keylift( unsigned char key, int x, int y ) {
  
  Camera &cam = myScreen.camList.Active();

  switch( key ) {
  case 'w':
    cam.Stop( Camera::Forward );
    break;
  case 's':
    cam.Stop( Camera::Backward );
    break;
  case 'a':
    cam.Stop( Camera::Left );
    break;
  case 'd':
    cam.Stop( Camera::Right );
    break;
  case 'q':
    cam.Stop( Camera::Up );
    break;
  case 'e':
    cam.Stop( Camera::Down );
    break;
  }
}

void keyboard( unsigned char key, int x, int y ) {

  /* A shorthand variable with local scope that refers to "The Active Camera." */
  Camera &cam = myScreen.camList.Active();

  switch( key ) {

  case 033: // Escape Key	  
    exit( EXIT_SUCCESS );
    break;
    
  case '+':
    cameraInit(myScreen.camList[myScreen.camList.addCamera()]);
    break;
  case '-':
    myScreen.camList.popCamera();
    break;
    
  case 'w':
    cam.Move( Camera::Forward );
    break;
  case 's':
    cam.Move( Camera::Backward );
    break;
  case 'a':
    cam.Move( Camera::Left );
    break;
  case 'd':
    cam.Move( Camera::Right );
    break;
  case 'q':
    cam.Move( Camera::Up );
    break;
  case 'e':
    cam.Move( Camera::Down );
    break;
    
  case ';': // Print Info
    fprintf( stderr, "Active Object: %s\n",
	     theScene.Active()->Name().c_str() );
    fprintf( stderr, "Camera Position: (%f,%f,%f)\n", cam.X(), cam.Y(), cam.Z() );
    break;
    
    //Perspectives
  case 'z': cam.changePerspective( Camera::PERSPECTIVE ); break;
  case 'x': cam.changePerspective( Camera::ORTHO ); break;
  case 'c': cam.changePerspective( Camera::ORTHO2D ); break;
  case 'v': cam.changePerspective( Camera::FRUSTUM ); break;
  case 'b': cam.changePerspective( Camera::IDENTITY ); break;

  case 'l':
    landGen( theScene["terrain"], 8, 40.0 );
    theScene["terrain"]->Buffer();
    break;

  }
}

void keyboard_ctrl( int key, int x, int y ) {
  switch (key) {
  case GLUT_KEY_PAGE_UP:
    myScreen.camList.Prev();
    break;

  case GLUT_KEY_PAGE_DOWN:
    myScreen.camList.Next();
    break;
   
  case GLUT_KEY_LEFT:
    theScene.Prev();
    break;
    
  case GLUT_KEY_RIGHT:
    theScene.Next();
    break;

  case GLUT_KEY_F1:
    theScene.Active()->Mode( GL_POINTS );
    break;
  case GLUT_KEY_F2:
    theScene.Active()->Mode( GL_LINE_STRIP );
    break;
  case GLUT_KEY_F3:
    theScene.Active()->Mode( GL_TRIANGLE_STRIP );
    break;
  case GLUT_KEY_F4:
    theScene.Active()->Mode( GL_TRIANGLES );
    break;
  }
}

void mouse( int button, int state, int x, int y ) {

  if ( state == GLUT_DOWN ) {
    switch( button ) {
    case 3: myScreen.camList.Active().dFOV( 1 ); break;
    case 4: myScreen.camList.Active().dFOV( -1 ); break;
    }
  }

}


void mouseroll( int x, int y ) {

  if ((x != myScreen.MidpointX()) || (y != myScreen.MidpointY())) {
    myScreen.camList.Active().roll( x - myScreen.MidpointX() );
    glutWarpPointer( myScreen.MidpointX(), myScreen.MidpointY() );
  }

}


void mouselook( int x, int y ) {

  if ((x != myScreen.MidpointX()) || (y != myScreen.MidpointY())) {
    const double dx = ((double)x - myScreen.MidpointX());
    const double dy = ((double)y - myScreen.MidpointY());
    
    myScreen.camList.Active().pitch( dy );
    myScreen.camList.Active().yaw( dx, fixed_yaw );
    
    glutWarpPointer( myScreen.MidpointX(), myScreen.MidpointY() );
  }
  
}


void resizeEvent( int width, int height ) {

  /* Handles resizing the child cameras as well. */
  myScreen.Size( width, height );
  glutWarpPointer( myScreen.MidpointX(), myScreen.MidpointY() );

}


void animationTest( TransCache &obj ) {

  //Object increasingly grows. 
  //obj.scale.Adjust( 1.001 );

  //Object rotates in-place.
  obj.rotation.RotateX( 0.1 );
  obj.rotation.RotateY( 0.1 );
  obj.rotation.RotateZ( 0.1 );

  //Object increasingly moves away from origin, x += 0.01
  obj.offset.Delta( 0.01, 0, 0 );

  //Object orbits about the origin
  obj.orbit.RotateX( 0.2 );
  obj.orbit.RotateY( 0.5 );
  obj.orbit.RotateZ( 0.3 );

  // Object moves its focal orbit-point, x = 5.
  //obj.displacement.Set( 5, 0, 0 );
  
}

void idle( void ) {

  static unsigned int frameNo = 0;

  Tick.Tock();
  //fprintf( stderr, "Time since last idle: %lu\n", Tick.Delta() );

  animationTest( theScene["pyramid"]->trans );
  theScene[ "pyramid" ]->trans.CalcCTM();


#ifdef WII
  if (usingWii) {
    for (size_t i = 0; i < 20; ++i) {
      pollWii( Wii );
      myScreen.camList.Active().Accel( bb_magnitudes );
    }
  }
#endif

  // Move all camera(s).
  myScreen.camList.IdleMotion();
  glutPostRedisplay();
    
}


//--------------------------------------------------------------------

void menufunc( int value ) {

  switch (value) {
  case 0:
    landGen( theScene["terrain"], 12, 40.0 );
    theScene["terrain"]->Buffer();
    break;
  case 1:
    if (fixed_yaw) fixed_yaw = false;
    else fixed_yaw = true;
    break;
  }
    
}

int main( int argc, char **argv ) {

#ifdef WII
  if (!(usingWii = initWii( Wii ))) {
    std::cerr << "Not using Wii controls for this runthrough.\n";
  }
#endif

  // OS X suppresses events after mouse warp.  This resets the suppression 
  // interval to 0 so that events will not be suppressed. This also found
  // at http://stackoverflow.com/questions/728049/
  // glutpassivemotionfunc-and-glutwarpmousepointer
#ifdef __APPLE__
  CGSetLocalEventsSuppressionInterval( 0.0 );
#endif

  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowSize( myScreen.Width(), myScreen.Height() );
  glutCreateWindow( "Gasket Flythrough" );
  glutFullScreen();
  glutSetCursor( GLUT_CURSOR_NONE );

  GLEW_INIT();
  init();

  /* Register our Callbacks */
  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );
  glutKeyboardUpFunc( keylift );
  glutSpecialFunc( keyboard_ctrl );
  glutMouseFunc( mouse );
  glutMotionFunc( mouseroll );
  glutPassiveMotionFunc( mouselook );
  glutIdleFunc( idle );
  glutReshapeFunc( resizeEvent );

  if (DEBUG) {
    fprintf( stderr, "GL_VENDOR: %s\n", glGetString( GL_VENDOR ));
    fprintf( stderr, "GL_RENDERER: %s\n", glGetString( GL_RENDERER ));
    fprintf( stderr, "GL_VERSION: %s\n", glGetString( GL_VERSION ));
    fprintf( stderr, "GL_SHADING_LANGUAGE_VERSION: %s\n", 
	     glGetString( GL_SHADING_LANGUAGE_VERSION ));
  }

  int menu = glutCreateMenu( menufunc );
  glutSetMenu( menu );
  glutAddMenuEntry( "Randomize Terrain", 0 );
  glutAddMenuEntry( "Toggle Free Rotation", 1 );
  glutAttachMenu( GLUT_RIGHT_BUTTON );

  /* PULL THE TRIGGER */
  glutMainLoop();
  return EXIT_SUCCESS;

}
