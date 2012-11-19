attribute vec4 vPosition;
attribute vec4 vColor;
varying   vec4 color;

uniform mat4 P;
uniform mat4 R;
uniform mat4 T;
uniform mat4 PRT;

void main() {

   color = vColor;

   #ifdef POSTMULT
   gl_Position = vPosition * T * R * P;
   #else
   gl_Position = PRT * vPosition;
   #endif

}
