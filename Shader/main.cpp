#include <stdio.h>
#include <stdlib.h>
#include <glew.h>
#include <GLUT/GLUT.h>
#include <vector>
#include "nvMath.h"

unsigned width = 1024;
unsigned height = 768;

unsigned shadowMapSize = 1024;

float theta = M_PI / 2;
float phi = M_PI / 2;
float camDist = 3;

nv::vec3f camFocus(0, 0, 0);
nv::vec3f camUp(0, 1, 0);

int mouseX, mouseY;

GLuint progID;

GLuint frameTexID = 0, frameBufferID = 0, depthBufferID = 0;

std::vector<GLuint> plShadowMaps;

void setFrameBuffer()
{
    if(frameTexID)
        glDeleteTextures(1, &frameTexID);
    
    if(frameBufferID)
        glDeleteFramebuffers(1, &frameBufferID);
    
    if(depthBufferID)
        glDeleteFramebuffers(1, &depthBufferID);
    
    glGenTextures(1, &frameTexID);
    glBindTexture(GL_TEXTURE_2D, frameTexID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glGenFramebuffers(1, &frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameTexID, 0);
    
    glGenRenderbuffers(1, &depthBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);
}

void setGeometry()
{
    glutSolidSphere(2, 100, 100);
}

void makeShadowmap(const nv::vec3f& position)
{
    GLuint newCubemapTexID = 0;
    
    if(depthBufferID)
        glDeleteFramebuffers(1, &depthBufferID);
    
    glGenTextures(1, &newCubemapTexID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, newCubemapTexID);
    glActiveTexture(GL_TEXTURE0+plShadowMaps.size());
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    for (unsigned face = 0; face < 6; face++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT24,
                     shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP, newCubemapTexID, 0);
    
    glGenRenderbuffers(1, &depthBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, shadowMapSize, shadowMapSize);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferID);
    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, drawBuffers);
    
    
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(0);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, float(width) / height, 0.1, 100);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt(position.x, position.y, position.z+1, position.x, position.y, position.z, 0, 1, 0);
    
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_DEPTH_TEST);
    
    glColor3f(1, 1, 1);
    
    setGeometry();
    
    glPopAttrib();
    
    plShadowMaps.push_back(newCubemapTexID);
}

void renderBitmapString(float x, float y, void *font,const char *string){
    const char *c;
    glRasterPos2i(x, y);
    for(c=string; *c != '\0'; c++)
        glutBitmapCharacter(font, *c);
}

std::vector<nv::vec3f> plPosList, dlPosList;
std::vector<nv::vec3f> plColorList, dlColorList;
nv::vec3f kd, ks, ka;
float sp;

void setUniformsPhong()
{
    nv::vec3f camPos = nv::vec3f(sin(phi)*cos(theta), cos(phi), sin(phi)*sin(theta)) * camDist;
    glUniform3fv(glGetUniformLocation(progID, "eyePos"), 1, (GLfloat*) camPos);
    glUniform3fv(glGetUniformLocation(progID, "kd"), 1, (GLfloat*) kd);
    glUniform3fv(glGetUniformLocation(progID, "ks"), 1, (GLfloat*) ks);
    glUniform3fv(glGetUniformLocation(progID, "ka"), 1, (GLfloat*) ka);
    glUniform1f(glGetUniformLocation(progID, "sp"), sp);
    glUniform1i(glGetUniformLocation(progID, "npl"), plPosList.size());
    glUniform1i(glGetUniformLocation(progID, "ndl"), dlPosList.size());
    char varName[128];
    for(unsigned i=0; i<plPosList.size(); i++)
    {
        sprintf(varName, "pp[%d]", i);
        glUniform3fv(glGetUniformLocation(progID, varName), 1, (GLfloat*) plPosList[i]);
        sprintf(varName, "pc[%d]", i);
        glUniform3fv(glGetUniformLocation(progID, varName), 1, (GLfloat*) plColorList[i]);
        sprintf(varName, "shadowMaps[%d]", i);
        glUniform1i(glGetUniformLocation(progID, varName), plShadowMaps[i]);
    }
    for(unsigned i=0; i<dlPosList.size(); i++)
    {
        sprintf(varName, "dd[%d]", i);
        glUniform3fv(glGetUniformLocation(progID, varName), 1, (GLfloat*) dlPosList[i]);
        sprintf(varName, "dc[%d]", i);
        glUniform3fv(glGetUniformLocation(progID, varName), 1, (GLfloat*) dlColorList[i]);
    }
}

void display(void)
{
    nv::vec4f rgba;
    
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(progID);
    setUniformsPhong();
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, float(width) / height, 0.1, 100);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    nv::vec3f camPos = nv::vec3f(sin(phi)*cos(theta), cos(phi), sin(phi)*sin(theta)) * camDist;
    gluLookAt(camPos.x, camPos.y, camPos.z, camFocus.x, camFocus.y, camFocus.z, camUp.x, camUp.y, camUp.z);
    
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_DEPTH_TEST);

    setGeometry();

    glPopAttrib();
    
    glReadPixels(mouseX-1, height - mouseY, 1, 1, GL_RGBA, GL_FLOAT, (GLfloat*)&rgba);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glColor3f(1, 1, 1);
    
    glBindTexture(GL_TEXTURE_2D, frameTexID);
    glEnable(GL_TEXTURE_2D);
    
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glTexCoord2f(1, 0);
    glVertex2f(width, 0);
    glTexCoord2f(1, 1);
    glVertex2f(width, height);
    glTexCoord2f(0, 1);
    glVertex2f(0, height);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    
    glColor3f(0, 1, 0);

    char string[512];
    
    sprintf(string, "rgba = (%f, %f, %f, %f)", rgba.x, rgba.y, rgba.z, rgba.w);
    
    renderBitmapString(20, 20, GLUT_BITMAP_9_BY_15, string);
    
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, width, height);
    
    setFrameBuffer();
}

void keyPress(unsigned char key, int x, int y)
{
    switch (key) {
        case 27:
            exit(0);
            break;
    }
}

float lastTime = 0;
unsigned outputFrameNumber = 0;

void idle()
{
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN)
    {
        mouseX = x;
        mouseY = y;
    }
}

void mouseDrag(int x, int y)
{
    theta += (x - mouseX) * 0.01;
    phi -= (y - mouseY) * 0.01;
    
    if(phi < M_PI / 12)
        phi = M_PI / 12;
    
    if(phi > M_PI * 11 / 12)
        phi = M_PI * 11 / 12;
    
    mouseX = x;
    mouseY = y;
}

static char* textFileRead(const char *fileName) {
    char* text;
    
    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");
        
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);
            
            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';
            }
            fclose(file);
        }
    }
    return text;
}

void setShaders()
{
    char *vs,*fs;
    
    GLuint v, f;
    
    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
    
    vs = textFileRead("../../ShaderFiles/phong.vert");
    fs = textFileRead("../../ShaderFiles/phong.frag");
    
    const char *vv = vs;
    const char *ff = fs;
    
    glShaderSource(v, 1, &vv, NULL);
    glShaderSource(f, 1, &ff, NULL);
    
    free(vs); free(fs);
    
    glCompileShader(v);
    GLint isCompiled = 0;
    glGetShaderiv(v, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(v, GL_INFO_LOG_LENGTH, &maxLength);
        
        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(v, maxLength, &maxLength, &errorLog[0]);
        
        printf("Vertex shader error:\n");
        printf("%s\n", errorLog.data());
        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(v); // Don't leak the shader.
        return;
    }
    // Shader compilation is successful.
    printf("Vertex shader is ready.\n");
    
    glCompileShader(f);
    glGetShaderiv(f, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(f, GL_INFO_LOG_LENGTH, &maxLength);
        
        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(f, maxLength, &maxLength, &errorLog[0]);
        
        printf("Fragment shader error:\n");
        printf("%s\n", errorLog.data());
        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(f); // Don't leak the shader.
        return;
    }
    // Shader compilation is successful.
    printf("Fragment shader is ready.\n");
    
    progID = glCreateProgram();
    
    glAttachShader(progID, v);
    glAttachShader(progID, f);
    
    glLinkProgram(progID);
    glUseProgram(progID);
}

void mouseMove(int x, int y)
{
    mouseX = x;
    mouseY = y;
}

int main(int argc, char** argv)
{
    for(int i=1; i<argc; i++)
    {
        if(strcmp(argv[i], "-kd") == 0)
        {
            kd.x = atof(argv[i+1]);
            kd.y = atof(argv[i+2]);
            kd.z = atof(argv[i+3]);
            i+=3;
        }
        
        if(strcmp(argv[i], "-ks") == 0)
        {
            ks.x = atof(argv[i+1]);
            ks.y = atof(argv[i+2]);
            ks.z = atof(argv[i+3]);
            i+=3;
        }
        if(strcmp(argv[i], "-ka") == 0)
        {
            ka.x = atof(argv[i+1]);
            ka.y = atof(argv[i+2]);
            ka.z = atof(argv[i+3]);
            i+=3;
        }
        if(strcmp(argv[i], "-ka") == 0)
        {
            ka.x = atof(argv[i+1]);
            ka.y = atof(argv[i+2]);
            ka.z = atof(argv[i+3]);
            i+=3;
        }
        if(strcmp(argv[i], "-sp") == 0)
        {
            sp = atof(argv[i+1]);
            i+=1;
        }
        if(strcmp(argv[i], "-pl") == 0)
        {
            plPosList.push_back(nv::vec3f(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3])));
            plColorList.push_back(nv::vec3f(atof(argv[i+4]), atof(argv[i+5]), atof(argv[i+6])));
            i+=6;
        }
        if(strcmp(argv[i], "-dl") == 0)
        {
            dlPosList.push_back(nv::vec3f(atof(argv[i+1]), atof(argv[i+2]), atof(argv[i+3])));
            dlColorList.push_back(nv::vec3f(atof(argv[i+4]), atof(argv[i+5]), atof(argv[i+6])));
            i+=6;
        }
    }
    
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    
    (void)glutCreateWindow("Shader");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyPress);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseDrag);
    glutPassiveMotionFunc(mouseMove);
    
    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
        printf("Ready for OpenGL 2.0\n");
    else
    {
        printf("OpenGL 2.0 not supported\n");
        exit(1);
    }
    
    setShaders();
    
    setFrameBuffer();
    
    for(unsigned i=0; i<plPosList.size(); i++)
        makeShadowmap(plPosList[i]);
    
    glutMainLoop();
    
    return EXIT_SUCCESS;
}