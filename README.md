# N64brew Game Jam 2024 Minigame Template

Welcome to this year's N64brew game jam. This year, we are creating a large collective minigame ROM, and that requires every project to use the same base. Therefore, we are providing this base template for all projects to fork from. Unfortunately, due to time constraints, we were only able to provide a single template for [Libdragon](https://github.com/DragonMinded/libdragon).

Please ensure that you are using the **latest** `preview` branch of Libdragon, that you have [Tiny3D](https://github.com/HailToDodongo/tiny3d) installed, and that you are able to compile this ROM without issues. This template has been tested for both OpenGL and [Tiny3D](https://github.com/HailToDodongo/tiny3d). We also recommend that you fork this repository so that you can then perform a pull request for your final project. 


### Starting a new minigame

To make things simple, every single minigame will be compiled into its own DSO file and loaded at runtime. In order to ensure that happens, each minigame should be placed in its own folder inside the `code`. Folder names should be alphanumeric. Inside this folder, you can create as many subfolders and code files as you need, be it C, C++, or header files. The makefile will automatically handle compiling of files for you.

In order for your minigame to work with the template's minigame manager, the following is required:
* You must have the following functions defined:
```c
/*==============================
    minigame_init
    The minigame initialization function
==============================*/
void minigame_init();

/*==============================
    minigame_fixedloop
    Code that is called every loop, at a fixed delta time.
    Use this function for stuff where a fixed delta time is 
    important, like physics.
    @param  The fixed delta time for this tick
==============================*/
void minigame_fixedloop(float deltatime);

/*==============================
    minigame_loop
    Code that is called every loop.
    @param  The delta time for this tick
==============================*/
void minigame_loop(float deltatime);

/*==============================
    minigame_cleanup
    Clean up any memory used by your game just before it ends.
==============================*/
void minigame_cleanup();
```
* You must have the following global struct defined:
```c
const MinigameDef minigame_def = {
    .gamename = "Example Game",
    .developername = "Your Name",
    .description = "This is an example game.",
    .instructions = "Press A to win."
};
```

Please be careful with cleaning up the memory used by your project, use the `sys_get_heap_stats` function provided by Libdragon to compare the head allocations during your minigame initialization and after everything has been cleaned. Libdragon does use `malloc` internally for handling some things, so if you notice that your cleanup function doesn't account for all bytes, try running your minigame two or three more times. The memory usage should stabilize after the first run of the minigame.

Both the `core.h` and `minigame.h` headers include some public functions which you should be using in your project. Most importantly, you should be using `core_get_playercontroller` to get a specific player's controller port, as there is no guarantee that player 1's controller is plugged into port 1 on the console.

If you are working on multiple minigames, **do not** cross reference files between them. For instance, if you create a function `myfunc` inside of a minigame, do not try to access `myfunc` in a separate minigame's codebase. You can duplicate the function for your other minigame without any problems, because the minigames are loaded at runtime and thus will not interfere with one another.

Regarding assets, to avoid name conflicts with other projects in the final ROM, you should create a folder for your specific minigame in the `assets` folder. The makefile will handle everything for you.

When in doubt, refer to how `examplegame` is done.


### Configuration during testing

`config.h` contains some configuration options inside it to help you test your project, and this is the only core file which you should be making any modifications to. You should avoid making **any changes** to the template itself. If you encounter a bug in the template, feel free to open an issue or create a pull request with a fix **so that said fix can be made available to all users**.