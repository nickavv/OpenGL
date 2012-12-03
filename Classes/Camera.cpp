#include <stdexcept>
#include <iostream>

#include "mat.hpp"
#include "vec.hpp"
#include "Camera.hpp"
#include "globals.h" //Math constants and macros (SQRT2, POW5)
using namespace Angel;

#ifdef POSTMULT
static const bool POSTMULT = true;
static const bool PREMULT = false;
#else
static const bool POSTMULT = false;
static const bool PREMULT = true;
#endif

/**
   comminInit is a private function that initializes local object attributes.
   It should be called by all available constructors.
   @return Void.
**/
void Camera::commonInit( void ) {
  for ( size_t i = (size_t)Begin;
	i != (size_t)End;
	++i) {
    Motion[i] = false;
  }
  this->speed = 0;
  this->speed_cap = 0;
  this->MaxAccel = 10;
  this->MaxSpeed = 200;
  this->FrictionMagnitude = 2;
  this->aspect = 1;
  this->currView = PERSPECTIVE;
  this->fovy = 45.0;
}

/**
   Initialization Constructor; sets the X,Y,Z coordinates explicitly.
   @param x The initial X coordinate.
   @param y The initial Y coordinate.
   @param z The initial Z coordinate.
**/
Camera::Camera( float x, float y, 
		float z ) {
  commonInit();
  this->pos( x, y, z, false );
}


/**
   Initialization Constructor, uses a vec3 as its initial coordinates.
   @param in A vec3 representing the initial coordinates.
**/
Camera::Camera( vec3 &in ) {
  commonInit();
  this->pos( in, false );
}


/**
   Initialization Constructor, uses a vec4 as its initial coordinates.
   @param in A vec4 representing the initial coordinates. The w component is ignored.
**/
Camera::Camera( vec4 &in ) {
  commonInit();
  this->pos( in, false );
}


/**
   Default destructor. Nothing of note.
**/
Camera::~Camera( void ) { }


/**
   Sets the X coordinate of the camera.
   @param in     The new X coordinate of the camera.
   @param update Whether or not to update the shader with the new coordinates.
   @return Void.
**/
void Camera::X( const float &in, const bool &update ) { 
  ctm[0][3] = T[0][3] = -in;
  if (update) send( TRANSLATION );
}


/**
   Sets the Y coordinate of the camera.
   @param in     The new Y coordinate of the camera.
   @param update Whether or not to update the shader with the new coordinates.
   @return Void.
**/
void Camera::Y( const float &in, const bool &update ) { 
  ctm[1][3] = T[1][3] = -in;
  if (update) send( TRANSLATION );
}


/**
   Sets the Z coordinate of the camera.
   @param in     The new Z coordinate of the camera.
   @param update Whether or not to update the shader with the new coordinates.
   @return Void.
**/
void Camera::Z( const float &in, const bool &update ) { 
  ctm[2][3] = T[2][3] = -in;
  if (update) send( TRANSLATION );
}


/**
   Sets the absolute position of the camera.
   @param x The new X coordinate of the camera.
   @param y The new Y coordinate of the camera.
   @param z The new Z coordinate of the camera.
   @param update Whether or not to update the shader with the new coordinates.
   @return Void.
**/
void Camera::pos( const float &x, const float &y, 
		  const float &z, const bool &update ) {
  X(x, false);
  Y(y, false);
  Z(z, false);
  if (update) send( TRANSLATION );
}


/**
   Sets the absolute position of the camera.
   @param in A vec4 containing the x, y, and z coordinates to set the camera to. The w coordinate is ignored.
   @param update Whether or not to update the shader with the new coordinates.
   @return Void.
**/
void Camera::pos( const vec4 &in, const bool &update ) {
  this->pos( in.x, in.y, in.z, update );
}


/**
   Sets the absolute position of the camera.
   @param in A vec3 containing the x, y, and z coordinates to set the camera to.
   @param update Whether or not to update the shader with the new coordinates.
   @return Void.
**/
void Camera::pos( const vec3 &in, const bool &update ) {
  this->pos( in.x, in.y, in.z, update );
}

  
/**
   Moves the camera along the X axis.
   @param by The float value of the X-axis displacement.
   @param update A boolean indicating whether or not to update the shader. 
   update defaults to true.
   @return void.
**/
void Camera::dX( const float &by, const bool &update ) { 
  X( X() + by, update ); 
}


/**
   Moves the camera along the Y axis.
   @param by The float value of the Y-axis displacement.
   @param update A boolean indicating whether or not to update the shader.
   update defaults to true.
   @return Void.
**/
void Camera::dY( const float &by, const bool &update ) {
  Y( Y() + by, update );
}


/**
   Moves the camera along the Z axis.
   @param by The float value of the Z-axis displacement.
   @param update A boolean indicating whether or not to update the shader.
   update defaults to true.
   @return Void.
**/
void Camera::dZ( const float &by, const bool &update ) { 
  Z( Z() + by, update ); 
}


/** 
    Moves the camera along the x, y, and z axes.
    @param x the X-axis displacement.
    @param y the Y-axis displacement.
    @param z the Z-axis displacement.
    @return Void.
**/
void Camera::dPos( const float &x, const float &y, 
		   const float &z ) { 
  dX( x, false );
  dY( y, false );
  dZ( z, false );
  send( TRANSLATION );
}


/** 
    Moves the camera along the x, y, and z axes.
    @param by A vec3 containing the X, Y, and Z axis displacements.
    @return Void.
**/
void Camera::dPos( const vec3 &by ) {
  dPos( by.x, by.y, by.z );
}


/** 
    Moves the camera along the x, y, and z axes.
    @param by A vec4 containing the X, Y, and Z axis displacements. The w component is ignored.
    @return Void.
**/
void Camera::dPos( const vec4 &by ) {
  dPos( by.x, by.y, by.z );
}


/** 
    adjustRotation is an internal function that rotates the camera.
    Technically, any transformation, not just a rotation, is possible.
    @param adjustment The 4x4 matrix to transform the CTM by.
    @return Void.
**/
void Camera::adjustRotation( const mat4 &adjustment, const bool &fixed ) {

  // By default, the 'order' bool represents the POSTMULT behavior.
  bool order = POSTMULT;
  // However, if the fixed bool is present, flip it around.
  if (fixed) order = !order;

  if (order) {
    // In a post-mult system, the argument order is left-to-right,
    // So the adjustment appears last.
    R = R * adjustment;
  } else {
    // In a pre-mult system, the last argument is applied first,
    // So the adjustment should appear first.    
    R = adjustment * R;
  }

  send( ROTATION );

}

/*void Camera::fixedRotation( const mat4 &adjustment ) {
#ifdef POSTMUST
  R = adjustment * R;
#else
  R = R * adjustment;
#endif
  send( ROTATION );
  }*/


/** 
  ROTATE_OFFSET is a macro which is used to normalize
  the six camera motion directions with respect to the
  current camera rotation. It is used in heave(), sway() and surge().
  @param V a vec4 representing the movement offset vector.
  @return A rotated vec4.
**/
#define ROTATE_OFFSET(V) (V * R)


/**
   Adjusts the camera's X coordinate relative to its current position.
   Negative values move the camera left,
   and positive values move the camera right.
   @param by The float to adjust the X coordinate by.
   @return Void.
**/
void Camera::sway( const float &by ) {
  dPos(ROTATE_OFFSET(vec4(by,0,0,0)));
}


/**
   Adjusts the camera's Z coordinate relative to its current position.
   Positive values move the camera forward,
   and negative values move the camera backward.
   Note that the camera uses model coordinates internally,
   so moving forward will increase the camera's Z position negatively.
   @param by The float to adjust the Z coordinate by.
   @return Void.
**/
void Camera::surge( const float &by ) {
  dPos(ROTATE_OFFSET(vec4(0,0,-by,0)));
}


/**
   Adjusts the camera's Y coordinate relative to its current position.
   Positive values move the camera up,
   and negative values move the camera down.
   @param by The float to adjust the Y coordinate by.
   @return Void.
**/
void Camera::heave( const float &by ) {
  dPos(ROTATE_OFFSET(vec4(0,by,0,0)));
}


/**
   pitch adjusts the X axis rotation; up/down look.
   A positive value represents looking up,
   while a negative value represents looking down.
   @param by A float, in degrees, to adjust the pitch by.
   @return Void.
**/
void Camera::pitch( const float &by, const bool &fixed ) {
  /*
    Since negative values are interpreted as pitching down,
    We leave the input uninverted, because a negative rotation
    about the X axis rotates counter-clockwise (looking right),
    and clockwise (looking left), which achieves the effect of
    looking 'down'.
  */ 
  adjustRotation(RotateX(-by), fixed);
}


/**
   yaw adjusts the Y axis rotation; left/right look.
   A positive value represents looking right,
   while a negative value represents looking left.
   @param by A float, in degrees, to adjust the yaw by.
   @return Void.
**/
void Camera::yaw( const float &by, const bool &fixed ) {
  /*
    Since a positive 'by' should represent looking right,
    we invert the rotation because rotating by a positive value
    will rotate right, which simulates looking left.
    Therefore, invert.
  */
  adjustRotation(RotateY(by), fixed);
}


/**
   roll adjusts the Z axis rotation; tilt or lean left/right.
   A positive value represents leaning right,
   while a negative value represents leaning left.
   @param by A float, in degrees, to adjust the roll by.
   @return Void.
**/
void Camera::roll( const float &by, const bool &fixed ) {
  adjustRotation(RotateZ(by), fixed);
}

/**
  Accel takes an input vec2 which represents an acceleration,
  and applies it to the motion vectors with regards to
  the maximum acceleration and the maximum speed of the camera.
  @param accel The vec2 which represents the (x,y) accel, where x,y are [-1,1].
  @return Void.
**/
void Camera::Accel( const vec3 &raw_accel ) {

  //Accel comes in as a vector with a length between -sqrt(2) and sqrt(2).
  //We change it to be between -MAX_ACCEL and MAX_ACCEL.
  vec3 accel = raw_accel * (MaxAccel/SQRT2);

  //Now, we scale our accel vector so that we accelerate less when we are near MaxSpeed.
  accel *= (1-POW5(speed_cap));
  
  //The acceleration is finally applied to the velocity vector.
  velocity += accel;

  //speed and speed_cap must now be recalculated.
  speed_cap = (speed = length(velocity))/MaxSpeed;

  if (0) fprintf( stderr, "Velocity: (%f,%f,%f)\n", velocity.x, velocity.y, velocity.z );
}


/**
   Move instructs the camera to begin moving in the specified direction. 
   @param Dir The direction in which to move.
     Can be any direction in the enumerated type Camera::Direction.
   @return Void.
**/
void Camera::Move( const Camera::Direction &Dir ) { 
  Motion[Dir] = true; 
}


/**
   Stop instructs the camera to stop moving in the specified direction.
   @param Dir The direction in which to stop moving.
   @return Void.
**/
void Camera::Stop( const Camera::Direction &Dir ) {
  Motion[Dir] = false;
}


/**
   Idle moves the camera forward in whichever directions it is
   configured to move in. Call it in the glut Idle function.
   @return Void.
**/
void Camera::Idle( void ) {

  /* Apply the automated motion instructions, if any --
     These are primarily from the keyboard. */
  if (Motion[Camera::Forward]) Accel(vec3(0,0,1));
  if (Motion[Camera::Backward]) Accel(vec3(0,0,-1));
  if (Motion[Camera::Right]) Accel(vec3(0,1,0));
  if (Motion[Camera::Left]) Accel(vec3(0,-1,0));
  if (Motion[Camera::Up]) Accel(vec3(1,0,0));
  if (Motion[Camera::Down]) Accel(vec3(-1,0,0));

  /* Apply the velocity vectors computed from Accel,
     which includes instructions from keyboard and the Balance Board. */

  heave( velocity.x / 20000 );
  sway( velocity.y / 20000 );
  surge( velocity.z / 20000 );

  // We can only apply friction if we are moving. 
  if (speed) {
    // Friction is a vector that is the opposite of velocity.
    vec3 frictionVec = -velocity;
    /* By dividing friction by (speed/FrictionMagnitude), 
       we guarantee that the magnitude is FrictionMagnitude. */
    frictionVec = frictionVec / (speed/FrictionMagnitude);
    velocity += frictionVec;
    speed = length(velocity);
    speed_cap = speed/MaxSpeed;
  }
}


/**
   X() returns the current position of the camera in model coordinates.
   @return The current X coordinate of the camera in model coordinates.
**/
float Camera::X( void ) const { return -T[0][3]; }


/**
   Y() returns the current position of the camera in model coordinates.
   @return The current Y coordinate of the camera in model coordinates.
**/
float Camera::Y( void ) const { return -T[1][3]; }


/**
   Z() returns the current position of the camera in model coordinates.
   @return The current Z coordinate of the camera in model coordinates.
**/
float Camera::Z( void ) const { return -T[2][3]; }


/**
   pos() gets the current camera position in model coordinates.
   @return A vec4 that represents the current camera coordinates.
**/
vec4 Camera::pos( void ) const { return vec4( X(), Y(), Z(), 1.0 ); }


/**
   FOV() gets the current camera Field-of-view angle.
   @return A float that is the y axis viewing angle.
**/
float Camera::FOV( void ) const { return fovy; }


/**
   FOV sets the current camera Field-of-view angle.
   This function will send the new perspective matrix to the shader.
   @param in The new field of view angle.
   @return Void.
**/
void Camera::FOV( const float &in ) { 
  fovy = in;
  if (currView == Camera::PERSPECTIVE)
    send( VIEW );
}


/**
   changePerspective changes the current perspective of the camera.
   @param vType Which perspective to use. see enum view_type for possibilities.
   @return Void.
**/
void Camera::changePerspective( const view_type &vType ) {
  
  currView = vType;
  refreshPerspective();

}


/**
   refreshPerspective re-generates the current view/perspective matrix of the camera.
   This function should be called after physical or virtual (viewport) screen resizes.
   @return Void.
**/
void Camera::refreshPerspective( void ) {
  
  // Some constants. For your pleasure.
  static const GLfloat zNear = 0.001;
  static const GLfloat zFar = 100.0;
  
  switch (currView) {
  case PERSPECTIVE:
    P = Perspective( fovy, aspect, zNear, zFar );
    break;
  case ORTHO:
    P = Ortho( -1.0, 1.0, -1.0, 1.0, zNear, zFar );
    break;
  case ORTHO2D:
    P = Ortho2D( -1.0, 1.0, -1.0, 1.0 );
    break;
  case FRUSTUM:
    P = Frustum( -1.0, 1.0, -1.0, 1.0, zNear, zFar );
    break;
  case IDENTITY:
  default:
    P = mat4( GLuint(1.0) );
    break;
  }
}
  

/**
   dFOV adjusts the field of view angle up or down by an amount.
   @param by The float to adjust the FOV angle by.
   @return Void.
**/
void Camera::dFOV( const float &by ) {
  FOV( FOV() + by );
}


/**
   viewport instructs this camera what his expected drawing window will be.
   This allows the camera to generate his viewing matrices with the
   correct aspect ratio.
   @param _X The X coordinate of the lower-left corner of our viewport.
   @param _Y the Y coordinate of the lower-left corner of our viewport.
   @param _Width The width of our viewport.
   @param _Height the height of our viewport.
   @return Void.
**/
void Camera::viewport( size_t _X, size_t _Y,
		       size_t _Width, size_t _Height ) {
  this->XPos = _X;
  this->YPos = _Y;
  this->width = _Width;
  this->height = _Height;
  this->aspect = (double)(this->width) / (double)(this->height);
  refreshPerspective();
}

/**
   send will send a glsl variable to the shader.
   @param which The parameter to send. Can be any from enum glsl_var.
   @return Void.
**/
void Camera::send( const glsl_var &which ) {

  switch (which) {
  case TRANSLATION:
    glUniformMatrix4fv( glsl_handles[which], 1, GL_FALSE, T );
    send( CTM );
    break;
  case ROTATION:
    glUniformMatrix4fv( glsl_handles[which], 1, GL_FALSE, R );
    send( CTM );
    break;
  case VIEW:
    glUniformMatrix4fv( glsl_handles[which], 1, GL_FALSE, P );
    send( CTM );
    break;
  case CTM:
#ifdef POSTMULT
    ctm = T*R*P;
#else
    ctm = P*R*T;
#endif
    glUniformMatrix4fv( glsl_handles[which], 1, GL_TRUE, ctm );
    break;
  default:
    throw std::invalid_argument( "Unknown GLSL variable handle." );
  }
}


/**
   Link associates the camera with a glsl uniform variable.
   @param program a GLuint handle to the shader application.
   @param which A glsl_var enumeration indication which variable to link.
   @param glslVarName The name of the variable in the shader.
   @return Void.
**/
void Camera::link( const GLuint &program, const glsl_var &which, 
		   const string &glslVarName ) {
  
  glsl_handles[which] = glGetUniformLocation( program, 
					      glslVarName.c_str() );
  send( which );

}

void Camera::Draw( void ) {

  glViewport( XPos, YPos, width, height );
  /* Send all of our matrices, who knows what the shader's gonna do with 'em */
  send( TRANSLATION );
  send( ROTATION );
  send( VIEW );
  send( CTM );

}
