
#include <stdio.h>


GLuint shaderProgram;
GLuint VAO;  


unsigned int readShaderFromFile(const char *par_filename, void *par_out)
{
    FILE *loc_fp;
    unsigned int loc_length = 0;
    unsigned char *loc_p = (unsigned char *)par_out;

    loc_fp = fopen(par_filename,"r");

    if(loc_fp != NULL)
    {
        fseek(loc_fp, 0, SEEK_END);
        loc_length = ftell(loc_fp);
        rewind(loc_fp);

        fread(loc_p, 1, loc_length, loc_fp);
        loc_p[loc_length] = '\0';
        fclose(loc_fp);
    }
    else
    {
        /*nothing to do*/
    }
    return loc_length;
}       


void compileShaders()
{
    GLint success;
    char infoLog[512];
    char loc_vertex[4096];
    unsigned int loc_vertex_length;
    const char *loc_sh = (char*)&loc_vertex[0];

    // Вершинный шейдер
    loc_vertex_length = readShaderFromFile(VERTEXSHADERSOURCE, &loc_vertex[0]);
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &loc_sh, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex Shader Error: %s\n", infoLog);
    }

    // Фрагментный шейдер
    loc_vertex_length = readShaderFromFile(FRAGMENTSHADERSOURCE, &loc_vertex[0]);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &loc_sh, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment Shader Error: %s\n", infoLog);
    }

    // Программа шейдера
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Shader Program Linking Error: %s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}      

