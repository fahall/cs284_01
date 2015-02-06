uniform vec3 eyePos;
varying vec3 normal, vertPos;
varying float position; //in WORLD coordinates
varying vec3 normal; //In WORLD coordinates
uniform vec3 ka; //Ambient Color
uniform vec3 kd; //Diffuse Color... could also be used for textures.
uniform vec3 ks; //Specular color
uniform float sp;   //Specular Exponent (aka shininess)
uniform vec3[5] pl; //Point Lights
uniform vec3[5] dl; //Directional Lights

void main()
{
    
    for (thisPointLight = 1; thisPointLight < 5; thisPointLight++)
    {
        vec3 inDir = normalize(lightPos - vertPos);
        vec3 outDir = normalize(eyePos - vertPos);
        vec3 halfVec = normalize(inDir + outDir);
        
        //Get Diffuse Component
        vec3 diffuse = dot(normalize(inDir))
    }
    gl_FragColor = vec4(0.5 * cos_term + 0.5 * pow(clamp(dot(halfVec, normal), 0.0, 1.0), 10.0));
}
