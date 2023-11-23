/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef LEO_MOCK_CHANNEL_H_
#define LEO_MOCK_CHANNEL_H_

#include <string>
#include <stdint.h>

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/channel.h"
#include "ns3/mobility-model.h"
#include "ns3/net-device.h"
#include "ns3/time-data-calculators.h"
#include "ns3/traced-callback.h"
#include "ns3/mobility-module.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "mock-net-device.h"
#include "mock-channel.h"

namespace ns3 {

/**
 * \ingroup network
 * \defgroup channel Channel
 */
/**
 * \ingroup channel
 * \brief Mocked satellite-gateway, satellite-terminal channel types
 *
 */
class LeoMockChannel : public MockChannel
{
public:
  static TypeId GetTypeId (void);

  LeoMockChannel ();
  virtual ~LeoMockChannel ();

  /**
   * \see MockChannel::TransmitStart
   *
   * A packet is transmitted if the destination is reachable via the beam.
   */
  virtual bool TransmitStart (Ptr<const Packet> p, uint32_t devId, Address dst, Time txTime);

protected:

  enum ChannelType
  {
    GW_FORWARD,
    GW_RETURN,
    UT_FORWARD,
    UT_RETURN,
    UNKNOWN
  };

  ChannelType GetChannelType () const;

private:
  /**
   * \brief Type of the channel
   */
  ChannelType m_channelType;

}; // class MockChannel

} // namespace ns3

#endif /* LEO_MOCK_CHANNEL_H */
