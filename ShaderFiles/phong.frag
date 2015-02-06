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
uniform vec3 pp[10]; //Point Lights
uniform vec3 pc[10]; //Point Lights
uniform vec3 dd[10]; //Directional Lights
uniform vec3 dc[10]; //Point Lights

uniform samplerCube shadowMaps[10];

void main()
{
    vec3 color = ka;
    
    for (int i = 0; i < npl; i++)
    {
        vec3 lightPos = pp[i];
        vec3 inDir = normalize(lightPos - vertPos);
        vec3 outDir = normalize(eyePos - vertPos);
        
        vec3 rDir = -inDir + 2.0 * clamp(dot(inDir, normal), 0.0, 1.0) * normal;
        
        vec3 diffuseColor = clamp(dot(inDir, normal), 0.0, 1.0) * kd;
        vec3 specColor = pow(clamp(dot(rDir, outDir), 0.0, 1.0), sp) * ks;
        if(dot(inDir, normal) < 0.0)
            specColor = vec3(0.0);
        color += (diffuseColor + specColor) * pc[i];
    }
    
    for (int i = 0; i < ndl; i++)
    {
        vec3 inDir = -normalize(dd[i]);
        vec3 outDir = normalize(eyePos - vertPos);
        
        vec3 rDir = -inDir + 2.0 * clamp(dot(inDir, normal), 0.0, 1.0) * normal;
        
        vec3 diffuseColor = clamp(dot(inDir, normal), 0.0, 1.0) * kd;
        vec3 specColor = pow(clamp(dot(rDir, outDir), 0.0, 1.0), sp) * ks;
        if(dot(inDir, normal) < 0.0)
            specColor = vec3(0.0);
        color += (diffuseColor + specColor) * dc[i];
    }
    
    //color = textureCube(shadowMaps[0], vec3(normal)).xyz;
    
    gl_FragColor = vec4(color, 1.0);
}
