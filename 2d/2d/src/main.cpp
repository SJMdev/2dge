#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <sol/sol.hpp>
#include "game/game.hpp"
#include <iostream>
void TestLua() {
    sol::state lua{};
    lua.open_libraries(sol::lib::base);
    lua.script_file("./assets/scripts/myscript.lua");
    int someVariableInsideCpp = lua["some_variable"];
    std::cout << "the value of some variable in C++ is " << someVariableInsideCpp << std::endl;

    // this is how easy we can get the values from inside a lua table.
    bool isFullscreen = lua["config"]["fullscreen"];


    sol::table config = lua["config"];
    int width = config["resolution"]["width"];
    int height = config["resolution"]["height"];
    std::cout << "we read the value fullscreen: " << isFullscreen << ", width: " << width << ", height: " << height << std::endl;


    sol::function fac = lua["factorial"];
    // fixed signature std::function<...>
    // can be used to tie a sol::function down
    std::function<int(int)> factorial = fac;
    int factorial_five = factorial(5);
    
    std::cout << "factorial five is: " << factorial_five << '\n';
    assert(factorial_five == 120);
}

int main(int argc, char* argv[]) {
    //auto game = Game{};
    //std::cerr << "Hello!" << '\n';

    //game.Initialize();
    //game.Run();
    //game.Destroy();

    TestLua();


    return 0;
}