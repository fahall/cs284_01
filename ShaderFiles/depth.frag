varying vec3 vertPos;
uniform vec3 eyePos;

void main()
{
    float d = length(eyePos - vertPos);
    gl_FragColor = vec4(d);
}
