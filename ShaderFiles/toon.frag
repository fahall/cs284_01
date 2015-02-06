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
        vec3 reflection = normalize((-1.0 * inDir) + (2.0 * (dot(inDir, normal)) * normal));
        
        vec3 rDir = -inDir + 2.0 * clamp(dot(inDir, normal), 0.0, 1.0) * normal;
        
        
        //Diffuse Calculations
        float diffuseIntensity = clamp(dot(inDir, normal), 0.0, 1.0);
        
        if (diffuseIntensity > 0.95)
        {
            diffuseIntensity = 1.0;
        }
        else if (diffuseIntensity > 0.5)
        {
            diffuseIntensity = 0.6;
        }
        else if (diffuseIntensity > 0.25)
        {
            diffuseIntensity = 0.4;
        }
        else
        {
            diffuseIntensity = 0.23;
        }
        vec3 diffuseColor = diffuseIntensity * kd;
        
        //Specular Calculations
        float specIntensity = pow(clamp(dot(rDir, outDir), 0.0, 1.0), sp);
        if(dot(inDir, normal) < 0.0)
            specIntensity = 0.0;
        
        if(specIntensity > 0.8)
            specIntensity = 1.0;
        else
            specIntensity = 0.0;
        
        vec3 specColor = specIntensity * ks;
        color += (diffuseColor + specColor) * pc[i];
    }
    
    for (int i = 0; i < ndl; i++)
    {
        vec3 inDir = -normalize(dd[i]);
        vec3 outDir = normalize(eyePos - vertPos);
        vec3 reflection = normalize((-1.0 * inDir) + (2.0 * (dot(inDir, normal)) * normal));
        
        vec3 rDir = -inDir + 2.0 * clamp(dot(inDir, normal), 0.0, 1.0) * normal;
        
        //Diffuse Calculations
        float diffuseIntensity = clamp(dot(inDir, normal), 0.0, 1.0);
        
        if (diffuseIntensity > 0.95)
        {
            diffuseIntensity = 1.0;
        }
        else if (diffuseIntensity > 0.5)
        {
            diffuseIntensity = 0.6;
        }
        else if (diffuseIntensity > 0.25)
        {
            diffuseIntensity = 0.4;
        }
        else
        {
            diffuseIntensity = 0.23;
        }
        vec3 diffuseColor = diffuseIntensity * kd;
        
        //Specular Calculations
        float specIntensity = pow(clamp(dot(rDir, outDir), 0.0, 1.0), sp);
        if(dot(inDir, normal) < 0.0)
            specIntensity = 0.0;
        
        if(specIntensity > 0.8)
            specIntensity = 1.0;
        else
            specIntensity = 0.0;
        
        vec3 specColor = specIntensity * ks;
        color += (diffuseColor + specColor) * pc[i];
    }
    
    //color = textureCube(shadowMaps[0], vec3(normal)).xyz;
    
    
    gl_FragColor = vec4(color, 1.0);
}