/*
 * PWM.cpp  Created on: 29 Apr 2014
 * Copyright (c) 2014 Derek Molloy (www.derekmolloy.ie)
 * Made available for the book "Exploring BeagleBone" 
 * See: www.exploringbeaglebone.com
 * Licensed under the EUPL V.1.1
 *
 * This Software is provided to You under the terms of the European 
 * Union Public License (the "EUPL") version 1.1 as published by the 
 * European Union. Any use of this Software, other than as authorized 
 * under this License is strictly prohibited (to the extent such use 
 * is covered by a right of the copyright holder of this Software).
 * 
 * This Software is provided under the License on an "AS IS" basis and 
 * without warranties of any kind concerning the Software, including 
 * without limitation merchantability, fitness for a particular purpose, 
 * absence of defects or errors, accuracy, and non-infringement of 
 * intellectual property rights other than copyright. This disclaimer 
 * of warranty is an essential part of the License and a condition for 
 * the grant of any rights to this Software.
 * 
 * For more details, see http://www.derekmolloy.ie/
 */

// Modified by BM on 2017-5-30

#include "PWM.h"
#include "util.h"
#include <cstdlib>

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cstdlib>
#include<cstdio>
//#include<fcntl.h>
#include<unistd.h>
//#include<sys/epoll.h>

using namespace std;
namespace exploringBB {

PWM::PWM(int number): UTIL(){   // UTIL parent class constructed before PWM child class
	this->number = number;
	this->analogFrequency = 100000;
	this->analogMax = 3.3;

	ostringstream s;
	s << "pwm" << number;
	this->name = string(s.str());
	this->path = PWM_PATH + this->name + "/";
	this->exportPWM();
		// need to give Linux time to set up the sysfs structure
		usleep(250000); // 250ms delay
}

int PWM::exportPWM(){
	return this->write(PWM_PATH, "export", this->number);
}

int PWM::unexportPWM(){
   return write(PWM_PATH, "unexport", this->number);
}

int PWM::setPeriod(unsigned int period_ns){
	return this->write(this->path, PWM_PERIOD, period_ns);
}

unsigned int PWM::getPeriod(){
	return atoi(this->read(this->path, PWM_PERIOD).c_str());
}

float PWM::period_nsToFrequency(unsigned int period_ns){
	float period_s = (float)period_ns/1000000000;
	return 1.0f/period_s;
}

unsigned int PWM::frequencyToPeriod_ns(float frequency_hz){
	float period_s = 1.0f/frequency_hz;
	return (unsigned int)(period_s*1000000000);
}

int PWM::setFrequency(float frequency_hz){
	return this->setPeriod(this->frequencyToPeriod_ns(frequency_hz));
}

float PWM::getFrequency(){
	return this->period_nsToFrequency(this->getPeriod());
}

int PWM::setDutyCycle(unsigned int duty_ns){
	return this->write(this->path, PWM_DUTY, duty_ns);
}

int PWM::setDutyCycle(float percentage){
	if ((percentage>100.0f)||(percentage<0.0f)) return -1;
	unsigned int period_ns = this->getPeriod();
	float duty_ns = period_ns * (percentage/100.0f);
	this->setDutyCycle((unsigned int) duty_ns );
	return 0;
}

unsigned int PWM::getDutyCycle(){
	return atoi(this->read(this->path, PWM_DUTY).c_str());
}

float PWM::getDutyCyclePercent(){
	unsigned int period_ns = this->getPeriod();
	unsigned int duty_ns = this->getDutyCycle();
	return 100.0f * (float)duty_ns/(float)period_ns;
}

int PWM::setPolarity(PWM::POLARITY polarity){
	return this->write(this->path, PWM_POLARITY, polarity);
}

void PWM::invertPolarity(){
	if (this->getPolarity()==PWM::ACTIVE_LOW) this->setPolarity(PWM::ACTIVE_HIGH);
	else this->setPolarity(PWM::ACTIVE_LOW);
}

PWM::POLARITY PWM::getPolarity(){
	if (atoi(this->read(this->path, PWM_POLARITY).c_str())==0) return PWM::ACTIVE_LOW;
	else return PWM::ACTIVE_HIGH;
}

int PWM::calibrateAnalogMax(float analogMax){ //must be between 3.2 and 3.4
	if((analogMax<3.2f) || (analogMax>3.4f)) return -1;
	else this->analogMax = analogMax;
	return 0;
}

int PWM::analogWrite(float voltage){
	if ((voltage<0.0f)||(voltage>3.3f)) return -1;
	this->setFrequency(this->analogFrequency);
	this->setPolarity(PWM::ACTIVE_LOW);
	this->setDutyCycle((100.0f*voltage)/this->analogMax);
	return this->run();
}

int PWM::run(){
	return this->write(this->path, PWM_RUN, 1);
}

bool PWM::isRunning(){
	string running = this->read(this->path, PWM_RUN);
	return (running=="1");
}

int PWM::stop(){
	return this->write(this->path, PWM_RUN, 0);
}

PWM::~PWM() {
	this->unexportPWM();
}

} /* namespace exploringBB */
