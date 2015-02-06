uniform vec3 eyePos;
varying vec3 normal, vertPos;
varying vec3 normal; //In WORLD coordinates
uniform vec3 ka; //Ambient Color
uniform vec3 kd; //Diffuse Color... could also be used for textures.
uniform vec3 ks; //Specular color
uniform float sp;   //Specular Exponent (aka shininess)
uniform vec3[5] pl; //Point Lights
uniform vec3[5] dl; //Directional Lights


void main()
{
    
    vec3 lightDir = vec3(1.0, 1.0, -0.50);
    float intensity;
    vec3 color = vec3(0.7, 0.7, 0.0);
    vec3 toonColor;
    intensity = dot(normalize(lightDir), normalize(normal));
    
    
    
    if (intensity > 0.95)
    {
        toonColor = color;
    }
    else if (intensity > 0.5)
    {
        toonColor = color * 0.6;
    }
    else if (intensity > 0.25)
    {
        toonColor = color * 0.4;
    }
    else
    {
        toonColor = color * 0.23;
    }
    
//    vec4 toonColor = vec4(color * toonBrightness, 1.0f);
    
    gl_FragColor = vec4(toonColor, 1.0);
    
}
