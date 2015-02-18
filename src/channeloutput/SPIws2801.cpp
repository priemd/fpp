/*
 *   WS2801 SPI handler for Falcon Player (FPP)
 *
 *   Copyright (C) 2013 the Falcon Player Developers
 *      Initial development by:
 *      - David Pitts (dpitts)
 *      - Tony Mace (MyKroFt)
 *      - Mathew Mrosko (Materdaddy)
 *      - Chris Pinkham (CaptainMurdoch)
 *      For additional credits and developers, see credits.php.
 *
 *   The Falcon Player (FPP) is free software; you can redistribute it
 *   and/or modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation; either version 2 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "log.h"
#include "settings.h"

#include "SPIws2801.h"

#ifdef USEWIRINGPI
#   include "wiringPi.h"
#   include "wiringPiSPI.h"
#else
#   define wiringPiSPISetup(a,b)    1
#   define wiringPiSPIDataRW(a,b,c) 1
#   define delayMicroseconds(a)     0
#endif

#define SPIWS2801_MAX_CHANNELS  1530

/////////////////////////////////////////////////////////////////////////////

/*
 *
 */
SPIws2801Output::SPIws2801Output(unsigned int startChannel, unsigned int channelCount)
  : ChannelOutputBase(startChannel, channelCount),
	m_port(-1)
{
	LogDebug(VB_CHANNELOUT, "SPIws2801Output::SPIws2801Output(%u, %u)\n",
		startChannel, channelCount);

	m_maxChannels = SPIWS2801_MAX_CHANNELS;
}

/*
 *
 */
SPIws2801Output::~SPIws2801Output()
{
	LogDebug(VB_CHANNELOUT, "SPIws2801Output::~SPIws2801Output()\n");
}

/*
 *
 */
int SPIws2801Output::Init(char *configStr)
{
	LogDebug(VB_CHANNELOUT, "SPIws2801Output::Init('%s')\n", configStr);

	vector<string> configElems = split(configStr, ';');

	for (int i = 0; i < configElems.size(); i++)
	{
		vector<string> elem = split(configElems[i], '=');
		if (elem.size() < 2)
			continue;

		if (elem[0] == "device")
		{
			LogDebug(VB_CHANNELOUT, "Using %s for SPI output\n",
				elem[1].c_str());

			if (elem[1] == "spidev0.0")
				m_port = 0;
			else if (elem[1] == "spidev0.1")
				m_port = 1;
		}
	}

	if (m_port == -1)
	{
		LogErr(VB_CHANNELOUT, "Invalid Config String: %s\n", configStr);
		return 0;
	}

	LogDebug(VB_CHANNELOUT, "Using SPI Port %d\n", m_port);

	if (wiringPiSPISetup(m_port, 1000000) < 0)
	{
		LogErr(VB_CHANNELOUT, "Unable to open SPI device\n") ;
		return 0;
	}

	return ChannelOutputBase::Init(configStr);
}


/*
 *
 */
int SPIws2801Output::Close(void)
{
	LogDebug(VB_CHANNELOUT, "SPIws2801Output::Close()\n");

	return ChannelOutputBase::Close();
}

/*
 *
 */
int SPIws2801Output::RawSendData(unsigned char *channelData)
{
	LogDebug(VB_CHANNELOUT, "SPIws2801Output::RawSendData(%p)\n", channelData);

	wiringPiSPIDataRW(m_port, (unsigned char *)channelData, m_channelCount);

	return m_channelCount;
}

/*
 *
 */
void SPIws2801Output::DumpConfig(void)
{
	LogDebug(VB_CHANNELOUT, "SPIws2801Output::DumpConfig()\n");

	LogDebug(VB_CHANNELOUT, "    port    : %d\n", m_port);

	ChannelOutputBase::DumpConfig();
}

