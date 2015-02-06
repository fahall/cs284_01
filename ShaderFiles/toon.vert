//Inspired by http://www.lighthouse3d.com/tutorials/glsl-tutorial/toon-shader-version-ii/

varying vec3 normal;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
    normal = gl_Normal;
}