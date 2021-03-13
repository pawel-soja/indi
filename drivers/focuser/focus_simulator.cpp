/*******************************************************************************
  Copyright(c) 2012 Jasem Mutlaq. All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License version 2 as published by the Free Software Foundation.
 .
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.
 .
 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
*******************************************************************************/

#include "focus_simulator.h"

#include <cmath>
#include <memory>
#include <cstring>
#include <unistd.h>

// We declare an auto pointer to focusSim.
static std::unique_ptr<FocusSim> focusSim(new FocusSim());

// Focuser takes 100 microsecond to move for each step, completing 100,000 steps in 10 seconds
#define FOCUS_MOTION_DELAY 100

void ISPoll(void *p);

void ISGetProperties(const char *dev)
{
    focusSim->ISGetProperties(dev);
}

void ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    focusSim->ISNewSwitch(dev, name, states, names, n);
}

void ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n)
{
    focusSim->ISNewText(dev, name, texts, names, n);
}

void ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
    focusSim->ISNewNumber(dev, name, values, names, n);
}

void ISNewBLOB(const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[],
               char *names[], int n)
{
    INDI_UNUSED(dev);
    INDI_UNUSED(name);
    INDI_UNUSED(sizes);
    INDI_UNUSED(blobsizes);
    INDI_UNUSED(blobs);
    INDI_UNUSED(formats);
    INDI_UNUSED(names);
    INDI_UNUSED(n);
}

void ISSnoopDevice(XMLEle *root)
{
    focusSim->ISSnoopDevice(root);
}

/************************************************************************************
 *
************************************************************************************/
FocusSim::FocusSim()
{
    FI::SetCapability(FOCUSER_CAN_ABS_MOVE | FOCUSER_CAN_REL_MOVE | FOCUSER_HAS_VARIABLE_SPEED | FOCUSER_HAS_BACKLASH);
}

/************************************************************************************
 *
************************************************************************************/
bool FocusSim::Connect()
{
    SetTimer(1000);
    return true;
}

/************************************************************************************
 *
************************************************************************************/
bool FocusSim::Disconnect()
{
    return true;
}

/************************************************************************************
 *
************************************************************************************/
const char *FocusSim::getDefaultName()
{
    return "Focuser Simulator";
}

/************************************************************************************
 *
************************************************************************************/
void FocusSim::ISGetProperties(const char *dev)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) != 0)
        return;

    INDI::Focuser::ISGetProperties(dev);

    defineProperty(ModeSP);
    loadConfig(true, "Mode");
}

/************************************************************************************
 *
************************************************************************************/
bool FocusSim::initProperties()
{
    INDI::Focuser::initProperties();

    SeeingNP[0].fill("SIM_SEEING", "arcseconds", "%4.2f", 0, 60, 0, 3.5);
    SeeingNP.fill(getDeviceName(), "SEEING_SETTINGS", "Seeing", MAIN_CONTROL_TAB, IP_RW, 60,
                       IPS_IDLE);

    FWHMNP[0].fill("SIM_FWHM", "arcseconds", "%4.2f", 0, 60, 0, 7.5);
    FWHMNP.fill(getDeviceName(), "FWHM", "FWHM", MAIN_CONTROL_TAB, IP_RO, 60, IPS_IDLE);

    TemperatureNP[0].fill("TEMPERATURE", "Celsius", "%6.2f", -50., 70., 0., 0.);
    TemperatureNP.fill(getDeviceName(), "FOCUS_TEMPERATURE", "Temperature",
                       MAIN_CONTROL_TAB, IP_RW, 0, IPS_IDLE);

    ModeSP[MODE_ALL].fill("All", "All", ISS_ON);
    ModeSP[MODE_ABSOLUTE].fill("Absolute", "Absolute", ISS_OFF);
    ModeSP[MODE_RELATIVE].fill("Relative", "Relative", ISS_OFF);
    ModeSP[MODE_TIMER].fill("Timer", "Timer", ISS_OFF);
    ModeSP.fill(getDeviceName(), "Mode", "Mode", MAIN_CONTROL_TAB, IP_RW,
                       ISR_1OFMANY, 60, IPS_IDLE);

    initTicks = sqrt(FWHMNP[0].value - SeeingNP[0].getValue()) / 0.75;

    FocusSpeedNP[0].setMin(1);
    FocusSpeedNP[0].setMax(5);
    FocusSpeedNP[0].setStep(1);
    FocusSpeedNP[0].setValue(1);

    FocusAbsPosNP[0].setValue(FocusAbsPosNP[0].getMax() / 2);

    internalTicks = FocusAbsPosNP[0].getValue();

    return true;
}

/************************************************************************************
 *
************************************************************************************/
bool FocusSim::updateProperties()
{
    INDI::Focuser::updateProperties();

    if (isConnected())
    {
        defineProperty(SeeingNP);
        defineProperty(FWHMNP);
        defineProperty(TemperatureNP);
    }
    else
    {
        deleteProperty(SeeingNP.getName());
        deleteProperty(FWHMNP.getName());
        deleteProperty(TemperatureNP.getName());
    }

    return true;
}

/************************************************************************************
 *
************************************************************************************/
bool FocusSim::ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        // Modes
        if (ModeSP.isNameMatch(name))
        {
            ModeSP.update(states, names, n);
            uint32_t cap = 0;
            int index    = ModeSP.findOnSwitchIndex();

            switch (index)
            {
                case MODE_ALL:
                    cap = FOCUSER_CAN_ABS_MOVE | FOCUSER_CAN_REL_MOVE | FOCUSER_HAS_VARIABLE_SPEED;
                    break;

                case MODE_ABSOLUTE:
                    cap = FOCUSER_CAN_ABS_MOVE;
                    break;

                case MODE_RELATIVE:
                    cap = FOCUSER_CAN_REL_MOVE;
                    break;

                case MODE_TIMER:
                    cap = FOCUSER_HAS_VARIABLE_SPEED;
                    break;

                default:
                    ModeSP.setState(IPS_ALERT);
                    ModeSP.apply("Unknown mode index %d", index);
                    return true;
            }

            FI::SetCapability(cap);
            ModeSP.setState(IPS_OK);
            ModeSP.apply();
            return true;
        }
    }

    return INDI::Focuser::ISNewSwitch(dev, name, states, names, n);
}

/************************************************************************************
 *
************************************************************************************/
bool FocusSim::ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        if (strcmp(name, "SEEING_SETTINGS") == 0)
        {
            SeeingNP.setState(IPS_OK);
            SeeingNP.update(values, names, n);

            SeeingNP.apply();
            return true;
        }

        if (strcmp(name, "FOCUS_TEMPERATURE") == 0)
        {
            TemperatureNP.setState(IPS_OK);
            TemperatureNP.update(values, names, n);

            TemperatureNP.apply();
            return true;
        }
    }

    // Let INDI::Focuser handle any other number properties
    return INDI::Focuser::ISNewNumber(dev, name, values, names, n);
}

/************************************************************************************
 *
************************************************************************************/
IPState FocusSim::MoveFocuser(FocusDirection dir, int speed, uint16_t duration)
{
    double mid         = (FocusAbsPosNP[0].max - FocusAbsPosNP[0].min) / 2;
    int mode           = ModeSP.findOnSwitchIndex();
    double targetTicks = ((dir == FOCUS_INWARD) ? -1 : 1) * (speed * duration);

    internalTicks += targetTicks;

    if (mode == MODE_ALL)
    {
        if (internalTicks < FocusAbsPosNP[0].min || internalTicks > FocusAbsPosNP[0].max)
        {
            internalTicks -= targetTicks;
            LOG_ERROR("Cannot move focuser in this direction any further.");
            return IPS_ALERT;
        }
    }

    // simulate delay in motion as the focuser moves to the new position
    usleep(duration * 1000);

    double ticks = initTicks + (internalTicks - mid) / 5000.0;

    FWHMNP[0].setValue(0.5625 * ticks * ticks + SeeingNP[0].getValue());

    LOGF_DEBUG("TIMER Current internal ticks: %g FWHM ticks: %g FWHM: %g", internalTicks, ticks,
               FWHMNP[0].getValue());

    if (mode == MODE_ALL)
    {
        FocusAbsPosNP[0].setValue(internalTicks);
        FocusAbsPosNP.apply();
    }

    if (FWHMNP[0].value < SeeingNP[0].getValue())
        FWHMNP[0].setValue(SeeingNP[0].value);

    FWHMNP.apply();

    return IPS_OK;
}

/************************************************************************************
 *
************************************************************************************/
IPState FocusSim::MoveAbsFocuser(uint32_t targetTicks)
{
    double mid = (FocusAbsPosNP[0].max - FocusAbsPosNP[0].min) / 2;

    internalTicks = targetTicks;

    // Limit to +/- 10 from initTicks
    double ticks = initTicks + (targetTicks - mid) / 5000.0;

    // simulate delay in motion as the focuser moves to the new position
    usleep(std::abs((int)(targetTicks - FocusAbsPosNP[0].getValue()) * FOCUS_MOTION_DELAY));

    FocusAbsPosNP[0].setValue(targetTicks);

    FWHMNP[0].setValue(0.5625 * ticks * ticks + SeeingNP[0].getValue());

    LOGF_DEBUG("ABS Current internal ticks: %g FWHM ticks: %g FWHM: %g", internalTicks, ticks,
               FWHMNP[0].getValue());

    if (FWHMNP[0].value < SeeingNP[0].getValue())
        FWHMNP[0].setValue(SeeingNP[0].value);

    FWHMNP.apply();

    return IPS_OK;
}

/************************************************************************************
 *
************************************************************************************/
IPState FocusSim::MoveRelFocuser(FocusDirection dir, uint32_t ticks)
{
    double mid = (FocusAbsPosNP[0].max - FocusAbsPosNP[0].min) / 2;
    int mode   = ModeSP.findOnSwitchIndex();

    if (mode == MODE_ALL || mode == MODE_ABSOLUTE)
    {
        uint32_t targetTicks = FocusAbsPosNP[0].getValue() + (ticks * (dir == FOCUS_INWARD ? -1 : 1));

        FocusAbsPosNP.setState(IPS_BUSY);
        FocusAbsPosNP.apply();

        return MoveAbsFocuser(targetTicks);
    }

    internalTicks += (dir == FOCUS_INWARD ? -1 : 1) * static_cast<int32_t>(ticks);

    ticks = initTicks + (internalTicks - mid) / 5000.0;

    LOGF_DEBUG("REL Current internal ticks: %g FWHM ticks: %g FWHM: %g", internalTicks, ticks,
               FWHMNP[0].getValue());

    FWHMNP[0].setValue(0.5625 * ticks * ticks + SeeingNP[0].getValue());

    if (FWHMNP[0].value < SeeingNP[0].getValue())
        FWHMNP[0].setValue(SeeingNP[0].value);

    FWHMNP.apply();

    return IPS_OK;
}

/************************************************************************************
 *
************************************************************************************/
bool FocusSim::SetFocuserSpeed(int speed)
{
    INDI_UNUSED(speed);
    return true;
}

/************************************************************************************
 *
************************************************************************************/
bool FocusSim::SetFocuserBacklash(int32_t steps)
{
    INDI_UNUSED(steps);
    return true;
}

/************************************************************************************
 *
************************************************************************************/
bool FocusSim::SetFocuserBacklashEnabled(bool enabled)
{
    INDI_UNUSED(enabled);
    return true;
}
