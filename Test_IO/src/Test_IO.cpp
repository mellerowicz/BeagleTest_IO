//============================================================================
// Name        : Test_IO.cpp
// Author      : Bolek Mellerowicz
// Version     :
// Copyright   : Your copyright notice
// Description : Code can be used to test GPIO on the BBB. Currently 4 outputs are
// used and one pwm channel. The am33xx_pwm device tree overlay must be loaded for the pwm
// to work:
// 				root@beaglebone:/lib/firmware# echo am33xx_pwm > $SLOTS
//
//============================================================================

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "GPIO.h"
#include "PWM.h"
using namespace exploringBB;
using namespace std;
#define NUM_OUTPUTS 5


int main() {

		// can be run by another user if udev rule is added (Molloy pg 242)
	if(getuid()!=0)
	{
		cout << "You must run this program as root. Exiting.";
		return -1;
	}
	//GPIO outGPIO(49); //inGPIO(115);
	//outGPIO.streamOpen();
	GPIO inGPIO(20, "MDCM FAULT");
	inGPIO.setActiveHigh();
	inGPIO.streamOpen();

	GPIO OUTPUTS[NUM_OUTPUTS]={{7, "SENSOR 5V SWITCH"},{27, "ESTOP"},{47, "PZ 5V SWITCH"},{49, "PZ HV SSR"},{115, "WATSON SSR"}};
	for(int i=0; i<NUM_OUTPUTS; i++) {
		OUTPUTS[i].setDirection(OUTPUT);
		OUTPUTS[i].setActiveHigh();
		OUTPUTS[i].streamOpen();
	}

	unsigned int dc = 250000;
	//PWM pwm("pwm_test_P9_14.13");
	PWM pwm(3);
	pwm.setPeriod(500000); 				 // period in ns
	pwm.setDutyCycle(dc);			 	 // duty cycle in ns
	//pwm.setPolarity(PWM::ACTIVE_HIGH);


	/// Command strings
	const std::string CMD_GET	            = "get";
	const std::string CMD_ON 	            = "on";
	const std::string CMD_OFF               = "off";
	const std::string CMD_PWM_ON 	        = "pon";
	const std::string CMD_PWM_OFF 	        = "poff";
	const std::string CMD_QUIT              = "quit";
	bool go = true;
	std::string         user_input;     // User input string

	for(int i=0; i<NUM_OUTPUTS; i++) {
		std::cout << "DO#: " << OUTPUTS[i].getNumber();
		std::cout << " - " << OUTPUTS[i].getDescription() << std::endl;
	}

	std::cout << "DI#: " << inGPIO.getNumber();
	std::cout << " - " << inGPIO.getDescription() << std::endl;

	std::cout << "PWM#: " << pwm.getName() << std::endl;

	while(go)
		{
			// Read command line
			std::cout << "GPIO >>: ";
			getline(std::cin, user_input);

			// PARSE INPUT ------------------------------------------------------------
			if(!user_input.compare(CMD_GET))
			{
				std::cout << "DI#: " << inGPIO.getNumber();
				std::cout << " - " << inGPIO.getValue() << std::endl;

				for(int i=0; i<NUM_OUTPUTS; i++) {
					std::cout << "DO#: " << OUTPUTS[i].getNumber();
					std::cout << " - " << OUTPUTS[i].getValue() << std::endl;
					}

				std::cout << "PWM#: " << pwm.getName() << " - " ;
				if(pwm.isRunning())
					std::cout << "1" << std::endl;
				else
					std::cout << "0" << std::endl;
			}

			else if(!user_input.compare(CMD_ON))
			{
				std::cout << "DO#: ";
				getline(std::cin, user_input);
				int gpio =  atoi(user_input.c_str());

				for(int i=0; i<NUM_OUTPUTS; i++) {
					if (gpio == OUTPUTS[i].getNumber())
						OUTPUTS[i].streamWrite(HIGH);
					}
			}
			else if(!user_input.compare(CMD_OFF))
			{
				std::cout << "DO#: ";
				getline(std::cin, user_input);
				int gpio =  atoi(user_input.c_str());

				for(int i=0; i<NUM_OUTPUTS; i++) {
					if (gpio == OUTPUTS[i].getNumber())
						OUTPUTS[i].streamWrite(LOW);
					}
				//outGPIO.streamWrite(LOW);
			}

			else if(!user_input.compare(CMD_PWM_ON))
			{
				pwm.run();
			}
			else if(!user_input.compare(CMD_PWM_OFF))
			{
				pwm.stop();
			}
			else if(!user_input.compare(CMD_QUIT))
			{
				go = false;
			}
			else{}
		}

	inGPIO.streamClose();

	for(int i=0; i<NUM_OUTPUTS; i++) {
		OUTPUTS[i].streamClose();
	}

	return 0;
}

/*#include<iostream>
#include"PWM.h"      //placed in the current directory for ease of use

using namespace exploringBB;
using namespace std;

int main(){
	PWM pwm("pwm_test_P9_14.13");  // example alternative pin
	//PWM pwm("pwm_test_P9_22.14");  // example alternative pin
   //PWM pwm("pwm_test_P9_42.12");  // P9_42 MUST be loaded as a slot before use
   pwm.setPeriod(100000);         // Set the period in ns
   pwm.setDutyCycle(25.0f);       // Set the duty cycle as a percentage
   pwm.setPolarity(PWM::ACTIVE_LOW);  // using active low PWM
   pwm.run();                     // start the PWM output
}*/

