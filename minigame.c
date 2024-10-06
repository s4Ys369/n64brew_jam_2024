#include <libdragon.h>
#include <string.h>
#include "core.h"
#include "minigame.h"

bool      global_minigame_ending = false;
Minigame* global_minigame_current = NULL;
Minigame* global_minigame_list;
size_t global_minigame_count;

const char* global_minigamepath = "rom:/minigames/";
const size_t global_minigamepath_len = 15;


void minigame_loadall()
{
    size_t gamecount = 0;
    dir_t minigamesdir;

    // First, go through the minigames path and count the number of minigames
    dir_findfirst(global_minigamepath, &minigamesdir);
    do
    {
        gamecount++;
    }
    while (dir_findnext("rom:/minigames/", &minigamesdir) == 0);
    global_minigame_count = gamecount/2; // Divide by two because we need to ignore the .sym files

    // Allocate the list of minigames
    global_minigame_list = (Minigame*)malloc(sizeof(Minigame));

    // Look through the minigames path and register all the known minigames
    gamecount = 0;
    dir_findfirst(global_minigamepath, &minigamesdir);
    do
    {
        void* handle;
        MinigameDef* loadeddef;
        Minigame* newdef = &global_minigame_list[gamecount];
        char* gamename = minigamesdir.d_name;
        char* fullpath = (char*)malloc(global_minigamepath_len + strlen(gamename) + 1);

        // Ignore the symbol file
        if (strstr(gamename, ".sym"))
            continue;

        // Get the filepath and open the dso
        sprintf(fullpath, "%s%s", global_minigamepath, gamename);
        handle = dlopen(fullpath, RTLD_LOCAL);

        // Get the symbols of the minigame definition. 
        // Since these symbols will only be temporarily stored in memory, we must make a deep copy
        loadeddef = dlsym(handle, "minigame_def");
        newdef->definition.gamename = (char*)malloc(strlen(loadeddef->gamename)+1);
        strcpy(newdef->definition.gamename, loadeddef->gamename);
        newdef->definition.developername = (char*)malloc(strlen(loadeddef->developername)+1);
        strcpy(newdef->definition.developername, loadeddef->developername);
        newdef->definition.description = (char*)malloc(strlen(loadeddef->description)+1);
        strcpy(newdef->definition.description, loadeddef->description);
        newdef->definition.instructions = (char*)malloc(strlen(loadeddef->instructions)+1);
        strcpy(newdef->definition.instructions, loadeddef->instructions);
        newdef->internalname = (char*)malloc(strlen(gamename) - 3);
        strncpy(newdef->internalname, gamename, strlen(gamename) - 4);

        // Cleanup
        dlclose(handle);
        free(fullpath);
        gamecount++;
    }
    while (dir_findnext("rom:/minigames/", &minigamesdir) == 0);
}

void minigame_play(char* name)
{
    const char* funcname_init = "_init";
    const char* funcname_fixedloop = "_fixedloop";
    const char* funcname_loop = "_loop";
    const char* funcname_cleanup = "_cleanup";
    char* fullpath = (char*)malloc(global_minigamepath_len + strlen(name) + 4 + 1);

    // Find the minigame with that name
    global_minigame_current = NULL;
    for (size_t i=0; i<global_minigame_count; i++)
    {
        debugf("Comparing %s to %s\n", global_minigame_list[i].internalname, name);
        if (!strcmp(global_minigame_list[i].internalname, name))
        {
            debugf("Success!\n");
            global_minigame_current = &global_minigame_list[i];
            break;
        }
    }
    assertf(global_minigame_current != NULL, "Unable to find minigame with internal name '%s'", name);

    // Load the dso and assign the internal functions
    debugf("Loading functions\n");
    sprintf(fullpath, "%s%s.dso", global_minigamepath, name);
    global_minigame_current->handle = dlopen(fullpath, RTLD_LOCAL);
    free(fullpath);
    fullpath = (char*)malloc(strlen(name) + strlen(funcname_init) + 1);
    sprintf(fullpath, "%s%s", name, funcname_init);
    global_minigame_current->funcPointer_init = dlsym(global_minigame_current->handle, fullpath);
    free(fullpath);
    fullpath = (char*)malloc(strlen(name) + strlen(funcname_fixedloop) + 1);
    sprintf(fullpath, "%s%s", name, funcname_fixedloop);
    global_minigame_current->funcPointer_loop = dlsym(global_minigame_current->handle, fullpath);
    free(fullpath);
    fullpath = (char*)malloc(strlen(name) + strlen(funcname_loop) + 1);
    sprintf(fullpath, "%s%s", name, funcname_loop);
    global_minigame_current->funcPointer_fixedloop = dlsym(global_minigame_current->handle, fullpath);
    free(fullpath);
    fullpath = (char*)malloc(strlen(name) + strlen(funcname_cleanup) + 1);
    sprintf(fullpath, "%s%s", name, funcname_cleanup);
    global_minigame_current->funcPointer_cleanup = dlsym(global_minigame_current->handle, fullpath);
    free(fullpath);
}

void minigame_end()
{
    global_minigame_ending = true;
}

Minigame* minigame_get_game()
{
    return global_minigame_current;
}

bool minigame_get_ended()
{
    return global_minigame_ending;
}

void minigame_cleanup()
{
    global_minigame_ending = false;
    dlclose(global_minigame_current->handle);
}