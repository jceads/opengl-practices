#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filePath) {

    std::ifstream stream(filePath);
    
    enum class ShaderType
    {
        NONE = -1,VERTEX = 0,FRAGMENT = 1
    };


    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) 
    {
        if (line.find("#shader") != std::string::npos) 
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }

    }

    return { ss[0].str(),ss[1].str() };
}

static unsigned int CompileShader (unsigned int type, const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src,nullptr);
    glCompileShader(id);
    
    
    int results;
    glGetShaderiv(id, GL_COMPILE_STATUS, &results);
    if (results == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message =(char*) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "failed to compile shader!" <<(type == GL_VERTEX_SHADER ?"vertex":"fragment")<<"shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;

    }



    return id;
}

static int createShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program =  glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER,fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);


    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;

}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }



    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;


    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;




    
    //we can call this buffer
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    //vertex buffer, kaç buffer lazým
    
    unsigned int buffer;
    glGenBuffers(1, &buffer);//buffer created
    glBindBuffer(GL_ARRAY_BUFFER,buffer);//buffer selected, selecting=binding
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);  //2.parametrede size belirtmemiz lazým

    glEnableVertexAttribArray(0);
    //2.parametre bir vertexin pozisyonun kaç sayý ile belirtirsin =2,Stride =>https://www.youtube.com/watch?v=x0H--CL2tUI&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&index=5 videoda anlatýyo
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float)*2,0);

    
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    std::cout << "Vertex \n";
    std::cout << source.VertexSource << std::endl;
    std::cout << "Fragment \n";
    std::cout << source.FragmentSource << std::endl;


        
    unsigned int shader = createShader(source.VertexSource,source.FragmentSource);
    glUseProgram(shader);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //glBegin(GL_TRIANGLES);
        //glVertex2f(-0.5f, -0.5f);
        //glVertex2f( 0.0f, 0.5f );
        //glVertex2f( 0.5f, -0.5f);
        //glEnd();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}
;
