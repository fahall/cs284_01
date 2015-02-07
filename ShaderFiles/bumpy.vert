varying vec3 normal, vertPos;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
    vertPos = gl_Vertex.xyz;
    float turbulence = rand(vec2(normal.x, normal.y));
    normal = gl_Normal * turbulence;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}