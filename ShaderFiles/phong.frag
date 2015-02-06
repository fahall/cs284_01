varying vec3 vertPos;
varying float position; //in WORLD coordinates
varying vec3 normal; //In WORLD coordinates

uniform vec3 eyePos;
uniform vec3 ka; //Ambient Color
uniform vec3 kd; //Diffuse Color... could also be used for textures.
uniform vec3 ks; //Specular color
uniform float sp;   //Specular Exponent (aka shininess)
uniform int npl; //Num of point Lights
uniform int ndl; //Num of dir Lights
uniform vec3 pl[10]; //Point Lights
uniform vec3 pc[10]; //Point Lights
uniform vec3 dl[10]; //Directional Lights
uniform vec3 dc[10]; //Point Lights

void main()
{
    vec3 color = ka;
    
    for (int i = 0; i < npl; i++)
    {
        vec3 lightPos = pl[i];
        vec3 inDir = normalize(lightPos - vertPos);
        vec3 outDir = normalize(eyePos - vertPos);
        vec3 halfVec = normalize(inDir + outDir);
        
        vec3 diffuseColor = dot(inDir, normal) * kd;
        vec3 specColor = pow(clamp(dot(halfVec, normal), 0.0, 1.0), sp) * ks;
        color += (diffuseColor + specColor) * pc[i];
    }
    
    for (int i = 0; i < ndl; i++)
    {
        vec3 inDir = -normalize(dl[i]);
        vec3 outDir = normalize(eyePos - vertPos);
        vec3 halfVec = normalize(inDir + outDir);
        
        vec3 diffuseColor = dot(inDir, normal) * kd;
        vec3 specColor = pow(clamp(dot(halfVec, normal), 0.0, 1.0), sp) * ks;
        color += (diffuseColor + specColor) * dc[i];
    }
    
    gl_FragColor = vec4(color, 1.0);
}
