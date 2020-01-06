/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include "Typedefs.h"
#include "Handler_Button.h"
#include "Handler_Display.h"

//Beginning of Auto generated function prototypes by Atmel Studio
void setup(void);
void loop(void);
//End of Auto generated function prototypes by Atmel Studio


void setup(void)
{
	Serial.begin(115200);
	Serial.println(tittle_text);
	handler_display_init();
	handler_button_init();
}

void loop(void)
{
	handler_button_mannager();
	handler_display_manager();
}
