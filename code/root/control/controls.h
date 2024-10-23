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
    data->pressed = joypad_get_buttons_pressed(core_get_playercontroller(PLAYER_1));
    data->held = joypad_get_buttons_held(core_get_playercontroller(PLAYER_1));
    data->released = joypad_get_buttons_released(core_get_playercontroller(PLAYER_1));
    data->input = joypad_get_inputs(core_get_playercontroller(PLAYER_1)); 
}

#endif