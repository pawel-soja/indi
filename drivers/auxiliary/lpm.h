/*******************************************************************************
  Copyright(c) 2019 Christian Liska. All rights reserved.

  INDI Astromechanic Light Pollution Meter Driver

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.

  The full GNU General Public License is included in this distribution in the
  file called LICENSE.
*******************************************************************************/

#pragma once

#include "defaultdevice.h"

/* Smart Widget-Property */
#include "indipropertytext.h"
#include "indipropertynumber.h"
#include "indipropertyswitch.h"

class LPM : public INDI::DefaultDevice
{
  public:
    LPM();
    virtual ~LPM();

    virtual bool initProperties() override;
    virtual bool updateProperties() override;
    virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;
    virtual bool ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n) override;

    /**
     * @struct LpmConnection
     * @brief Holds the connection mode of the device.
     */
    enum
    {
        CONNECTION_NONE   = 1 << 0,
        CONNECTION_SERIAL = 1 << 1
    } LpmConnection;

    enum
    {
        SAVE_READINGS,
        DISCARD_READINGS
    };

  protected:
    const char *getDefaultName();
    void TimerHit();

  private:
    bool getReadings();
    bool getDeviceInfo();
    void openFilePtr();

    // Readings
    INDI::PropertyNumber AverageReadingNP {5};

    // Record File Info
    INDI::PropertyText RecordFileTP {2};

    INDI::PropertySwitch ResetBP {1};

    INDI::PropertySwitch SaveBP {2};

    // Device Information
    INDI::PropertyNumber UnitInfoNP {1};

    Connection::Serial *serialConnection { nullptr };

    int PortFD { -1 };
    long count = 0;
    float sumSQ = 0;
    char filename[2048];

    uint8_t lpmConnection { CONNECTION_SERIAL };

    FILE * fp = nullptr;
};
