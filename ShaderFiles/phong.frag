uniform vec3 lightPos, eyePos;
varying vec3 normal, vertPos;

void main()
{
    vec3 inDir = normalize(lightPos - vertPos);
    vec3 outDir = normalize(eyePos - vertPos);
    float cos_term = clamp(dot(normal, inDir), 0.0, 1.0);
    vec3 halfVec = normalize(inDir + outDir);
    gl_FragColor = vec4(0.5 * cos_term + 0.5 * pow(clamp(dot(halfVec, normal), 0.0, 1.0), 10.0));
}
