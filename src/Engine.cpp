#include "Engine.h"
#include "Cube.h"
#define GLT_IMPLEMENTATION
#include "gltext.h"
#include <sstream>  
#include <iomanip>

void Engine::Run(){

    Window::Init((int)(1920 * 0.75f),(int)(1920 * 0.75f));
    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    
    Cube cube;
    
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    gltInit();

    GLTtext *text1 = gltCreateText();
    gltSetText(text1, "CamPos: (0.0, 0.0, 0.0)"); 
    
    while (Window::WindowIsOpen() && Window::WindowHasNotBeenForceClosed()){  
        Window::ShowFPS();
        Window::DeltaTime();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube.SetupCameraUniforms(Window::_camera, static_cast<float>(Window::width / Window::height));

        for(int i = 0; i < 10; i++){
            cube.Render(Window::_camera, cubePositions[i]);
        }

        // Update the camera position text
        glm::vec3 cameraPosition = Window::_camera.Position;  
        std::ostringstream oss;
        oss << "CamPos: (" 
            << std::fixed << std::setprecision(2) 
            << cameraPosition.x << ", "
            << cameraPosition.y << ", "
            << cameraPosition.z << ")";

        gltSetText(text1, oss.str().c_str());  

        glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                // Draw text
                gltBeginDraw();
                    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
                    gltDrawText2D(text1, 0.0f, 0.0f, 2.0f); 
                gltEndDraw();

            // Disable blending for 3D rendering
            glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);

        if (Input::KeyPressed(KEY_F)) Window::ToggleFullscreen();   
    
        if (Input::KeyPressed(KEY_H)) Window::ToggleWireframe();
        

        Window::ProcessInput();
        Input::Update();
        Window::SwapBuffersPollEvents();
    }
}
