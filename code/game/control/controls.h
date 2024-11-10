#ifndef CONTROLS_H
#define CONTROLS_H


typedef struct {

    joypad_buttons_t pressed;
    joypad_buttons_t held;
    joypad_buttons_t released;
    joypad_inputs_t input;

} ControllerData;


void controllerData_getInputs(ControllerData* data);

void controllerData_getInputs(ControllerData* data)
{
    joypad_poll();
    
    for (int i = 0; i < PLAYER_COUNT; i++) {
        
        data[i].pressed = joypad_get_buttons_pressed(i);
        data[i].held = joypad_get_buttons_held(i);
        data[i].released = joypad_get_buttons_released(i);
        data[i].input = joypad_get_inputs(i); 
    }
}

#endif