varying vec3 vertPos;

void main()
{
    vertPos = gl_Vertex.xyz / gl_Vertex.w;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}