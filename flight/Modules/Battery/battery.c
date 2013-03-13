/**
 ******************************************************************************
 * @addtogroup OpenPilotModules OpenPilot Modules
 * @{
 * @addtogroup BatteryModule Battery Module
 * @brief Measures battery voltage and current
 * Updates the FlightBatteryState object
 * @{
 *
 * @file       battery.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Module to read the battery Voltage and Current periodically and set alarms appropriately.
 *
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/**
 * Output object: FlightBatteryState
 *
 * This module will periodically generate information on the battery state.
 *
 * UAVObjects are automatically generated by the UAVObjectGenerator from
 * the object definition XML file.
 *
 * Modules have no API, all communication to other modules is done through UAVObjects.
 * However modules may use the API exposed by shared libraries.
 * See the OpenPilot wiki for more details.
 * http://www.openpilot.org/OpenPilot_Application_Architecture
 *
 */

#include "openpilot.h"

#include "flightbatterystate.h"
#include "flightbatterysettings.h"
#include "modulesettings.h"
#include "adcrouting.h"

//
// Configuration
//
#define SAMPLE_PERIOD_MS		500
// Private types

// Private variables
static bool module_enabled = false;

//THESE COULD BE BETTER AS SOME KIND OF UNION OR STRUCT, BY WHICH 4 BITS ARE USED FOR EACH 
//PIN VARIABLE, ONE OF WHICH INDICATES SIGN, AND THE OTHER 3 BITS INDICATE POSITION. THIS WILL
//WORK FOR QUITE SOMETIME, UNTIL MORE THAN 8 ADC ARE AVAILABLE. EVEN AT THIS POINT, THE STRUCTURE 
//CAN SIMPLY BE MODIFIED TO SUPPORT 15 ADC PINS, BY USING ALL AVAILABLE BITS.
static int8_t voltageADCPin=-1; //ADC pin for voltage
static int8_t currentADCPin=-1; //ADC pin for current

// Private functions
static void onTimer(UAVObjEvent* ev);

/**
 * Initialise the module, called on startup
 * \returns 0 on success or -1 if initialisation failed
 */
int32_t BatteryInitialize(void)
{
	ADCRoutingInitialize();
	
#ifdef MODULE_Battery_BUILTIN
	module_enabled = true;
#else
	uint8_t module_state[MODULESETTINGS_ADMINSTATE_NUMELEM];
	ModuleSettingsAdminStateGet(module_state);
	if (module_state[MODULESETTINGS_ADMINSTATE_BATTERY] == MODULESETTINGS_ADMINSTATE_ENABLED) {
		module_enabled = true;
	} else {
		module_enabled = false;
	}
#endif

	uint8_t adc_channel_map[ADCROUTING_CHANNELMAP_NUMELEM];	
	ADCRoutingChannelMapGet(adc_channel_map);
	
	//Determine if the battery sensors are routed to ADC pins 
	for (int i = 0; i < ADCROUTING_CHANNELMAP_NUMELEM; i++) {
		if (adc_channel_map[i] == ADCROUTING_CHANNELMAP_BATTERYVOLTAGE) {
			voltageADCPin = i;
		}
		if (adc_channel_map[i] == ADCROUTING_CHANNELMAP_BATTERYCURRENT) {
			currentADCPin = i;
		}
	}
	
	//Don't enable module if no ADC pins are routed to the sensors
	if (voltageADCPin <0 && currentADCPin <0)
		module_enabled = false;

	//Start module
	if (module_enabled) {
		FlightBatteryStateInitialize();
		FlightBatterySettingsInitialize();
	
		static UAVObjEvent ev;

		memset(&ev,0,sizeof(UAVObjEvent));
		EventPeriodicCallbackCreate(&ev, onTimer, SAMPLE_PERIOD_MS / portTICK_RATE_MS);
	}

	return 0;
}

MODULE_INITCALL(BatteryInitialize, 0)
#define HAS_SENSOR(x) batterySettings.SensorType[x]==FLIGHTBATTERYSETTINGS_SENSORTYPE_ENABLED 
static void onTimer(UAVObjEvent* ev)
{
	static FlightBatteryStateData flightBatteryData;
	FlightBatterySettingsData batterySettings;

	FlightBatterySettingsGet(&batterySettings);

	static float dT = SAMPLE_PERIOD_MS / 1000.0f;
	float energyRemaining;

	//calculate the battery parameters
	if (voltageADCPin >=0) {
		flightBatteryData.Voltage = ((float)PIOS_ADC_PinGet(voltageADCPin)) * batterySettings.SensorCalibrations[FLIGHTBATTERYSETTINGS_SENSORCALIBRATIONS_VOLTAGEFACTOR]; //in Volts
	}
	else {
		flightBatteryData.Voltage=1234; //Dummy placeholder value. This is in case we get another source of battery current which is not from the ADC
	}

	if (currentADCPin >=0) {
		flightBatteryData.Current = ((float)PIOS_ADC_PinGet(currentADCPin)) * batterySettings.SensorCalibrations[FLIGHTBATTERYSETTINGS_SENSORCALIBRATIONS_CURRENTFACTOR]; //in Amps
		if (flightBatteryData.Current > flightBatteryData.PeakCurrent) 
			flightBatteryData.PeakCurrent = flightBatteryData.Current; //in Amps
	}
	else { //If there's no current measurement, we still need to assign one. Make it negative, so it can never trigger an alarm
		flightBatteryData.Current=-0.1234f; //Dummy placeholder value. This is in case we get another source of battery current which is not from the ADC
	}
	
	flightBatteryData.ConsumedEnergy += (flightBatteryData.Current * dT * 1000.0f / 3600.0f) ;//in mAh
	
	//Apply a 2 second rise time low-pass filter to average the current
	float alpha = 1.0f-dT/(dT+2.0f);
	flightBatteryData.AvgCurrent=alpha*flightBatteryData.AvgCurrent+(1-alpha)*flightBatteryData.Current; //in Amps

	/*The motor could regenerate power. Or we could have solar cells. 
	 In short, is there any likelihood of measuring negative current? If it's a bad current reading we want to check, then 
	 it makes sense to saturate at max and min values, because a misreading could as easily be very large, as negative. The simple
	 sign check doesn't catch this.*/
//	//sanity checks 
//	if (flightBatteryData.AvgCurrent<0) flightBatteryData.AvgCurrent=0.0f;
//	if (flightBatteryData.PeakCurrent<0) flightBatteryData.PeakCurrent=0.0f;
//	if (flightBatteryData.ConsumedEnergy<0) flightBatteryData.ConsumedEnergy=0.0f;

	energyRemaining = batterySettings.Capacity - flightBatteryData.ConsumedEnergy; // in mAh
	if (flightBatteryData.AvgCurrent > 0)
		flightBatteryData.EstimatedFlightTime = (energyRemaining / (flightBatteryData.AvgCurrent*1000.0f))*3600.0f;//in Sec
	else
		flightBatteryData.EstimatedFlightTime = 9999;

	//generate alarms where needed...
	if ((flightBatteryData.Voltage<=0) && (flightBatteryData.Current<=0))
	{ 
		//FIXME: There's no guarantee that a floating ADC will give 0. So this 
		// check might fail, even when there's nothing attached.
		AlarmsSet(SYSTEMALARMS_ALARM_BATTERY, SYSTEMALARMS_ALARM_ERROR);
		AlarmsSet(SYSTEMALARMS_ALARM_FLIGHTTIME, SYSTEMALARMS_ALARM_ERROR);
	}
	else
	{
		// FIXME: should make the timer alarms user configurable
		if (flightBatteryData.EstimatedFlightTime < 30) 
			AlarmsSet(SYSTEMALARMS_ALARM_FLIGHTTIME, SYSTEMALARMS_ALARM_CRITICAL);
		else if (flightBatteryData.EstimatedFlightTime < 120) 
			AlarmsSet(SYSTEMALARMS_ALARM_FLIGHTTIME, SYSTEMALARMS_ALARM_WARNING);
		else 
			AlarmsClear(SYSTEMALARMS_ALARM_FLIGHTTIME);

		// FIXME: should make the battery voltage detection dependent on battery type. 
		/*Not so sure. Some users will want to run their batteries harder than others, so it should be the user's choice. [KDS]*/
		if (flightBatteryData.Voltage < batterySettings.VoltageThresholds[FLIGHTBATTERYSETTINGS_VOLTAGETHRESHOLDS_ALARM])
			AlarmsSet(SYSTEMALARMS_ALARM_BATTERY, SYSTEMALARMS_ALARM_CRITICAL);
		else if (flightBatteryData.Voltage < batterySettings.VoltageThresholds[FLIGHTBATTERYSETTINGS_VOLTAGETHRESHOLDS_WARNING])
			AlarmsSet(SYSTEMALARMS_ALARM_BATTERY, SYSTEMALARMS_ALARM_WARNING);
		else 
			AlarmsClear(SYSTEMALARMS_ALARM_BATTERY);
	}
	
	FlightBatteryStateSet(&flightBatteryData);
}

/**
  * @}
  */

/**
 * @}
 */
