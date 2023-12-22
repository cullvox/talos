#include "App.h"

ts::App app;

SET_LOOP_TASK_STACK_SIZE(1024*16)

void setup()
{
    app.init();
}

void loop() 
{
}
