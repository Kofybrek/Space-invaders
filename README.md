# Space-invaders
Remake of the original Space Invaders game.

Here's the video explaining how I did it: https://youtu.be/WfYNelQiQvc

## Compile instructions
* If you're using Visual Studio:
    Visual studio can handle CMakeFiles automatically: see [this link](https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-170)
* If you're on Linux:
    1) Install SFML:

        * *Debian/Ubuntu*: `sudo apt-get install libsfml-dev`
        * *Arch Linux and derivatives*: `sudo pacman -S sfml`
    2) Compile the project:
        
        In the root folder:
        ```
        mkdir build && cd build
        cmake ..
        make
        ```
    You should now have an executable file called `invaders` which you can run