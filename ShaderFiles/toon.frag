varying float position;
varying vec3 normal;
uniform vec3 ka, kd, ks;
uniform float sp;
uniform vec3[5] pl;
uniform vec3[5] dl;


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
