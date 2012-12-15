// attributes
attribute vec4 vPosition;
attribute vec4 vColor;
attribute vec2 vTex;

// sent to the fshader
varying vec4 color;
varying vec4 fPosition;
varying vec2 outtexture;

// position/movement
uniform mat4 P;
uniform mat4 R;
uniform mat4 T;
uniform mat4 CTM;
uniform mat4 OTM;
uniform bool fIsTextured;

void main() {

     //gl_Position = OTM * CTM * vPosition;
     //gl_Position = P * OTM * R * T * vPosition;
     gl_Position = P * R * T * OTM * vPosition;
     color = vColor;
     if (fIsTextured) outtexture = vTex;
     //else outtexture = vec2( -1.0, -1.0 );
     fPosition = vPosition;

}
