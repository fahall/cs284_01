varying vec3 vertPos;

void main()
{
    vertPos = gl_Vertex.xyz;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}