/**
 * @file Transformation.hpp
 * @author John Huston
 * @date 2013-03-29
 * @brief Headers for Transformation, RotMat, TransMat and ScaleMat.
 * @details Headers for the Transformation superclass, and Rotation,
 * Translation, and Scale specialization classes.
 */

#ifndef __TRANSMAT_HPP
#define __TRANSMAT_HPP

#include "mat.hpp"
#include "vec.hpp"
#include "platform.h" //OpenGL types.
class Transformation {
  
public:
  /*
   These are all default and are not currently needed.
   Transformation( void );
   Transformation( const Transformation &copy );
   Transformation &operator=( const Transformation &assignment );
   */
  virtual ~Transformation( void );

  const Angel::mat4 &matrix( void ) const;
  Angel::mat4 operator*( const Angel::mat4 &rhs ) const;
  Angel::mat4 operator*( const Transformation &rhs ) const;

protected:
  Angel::mat4 mat;
  
};

Angel::mat4 operator*( const Angel::mat4 &lhs, const Transformation &rhs );

/** Rotations **/

class RotMat : public Transformation {
  
public:
  
  const RotMat &reset( const Angel::mat4 &NewState );
  const RotMat &rotateX( const GLfloat theta, bool postmult = true );
  const RotMat &rotateY( const GLfloat theta, bool postmult = true );
  const RotMat &rotateZ( const GLfloat theta, bool postmult = true );
  const RotMat &adjust( const Angel::mat4 &Adjustment, bool postmult = true );
  
};

/** Translations **/

class TransMat : public Transformation {
  
public:
  
  const TransMat &setX( const float x );
  const TransMat &setY( const float y );
  const TransMat &setZ( const float z );

  const TransMat &set( const float x, const float y, const float z );
  const TransMat &set( const Angel::vec3 &arg );

  const TransMat &delta( const float x, const float y, const float z );
  const TransMat &delta( const Angel::vec3 &arg );
  
};

class ScaleMat : public Transformation {
  
public:
  
  const ScaleMat &set( const float x, const float y, const float z );
  const ScaleMat &set( const float pct );

  const ScaleMat &adjust( const float x, const float y, const float z );
  const ScaleMat &adjust( const float pct );
  
};

#endif
