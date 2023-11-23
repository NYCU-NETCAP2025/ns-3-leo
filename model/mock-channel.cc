/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007, 2008 University of Washington
 *
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

#include <ns3/trace-source-accessor.h>
#include <ns3/packet.h>
#include <ns3/simulator.h>
#include <ns3/log.h>
#include <ns3/pointer.h>
#include "mock-channel.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MockChannel");

NS_OBJECT_ENSURE_REGISTERED (MockChannel);

TypeId
MockChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MockChannel")
    .SetParent<Channel> ()
    .SetGroupName ("Leo")
    .AddAttribute ("PropagationDelay",
                   "A propagation delay model for the channel.",
                   PointerValue (),
                   MakePointerAccessor (&MockChannel::m_propagationDelay),
                   MakePointerChecker<PropagationDelayModel> ())
    .AddAttribute ("PropagationLoss",
                   "A propagation loss model for the channel.",
                   PointerValue (),
                   MakePointerAccessor (&MockChannel::m_propagationLoss),
                   MakePointerChecker<PropagationLossModel> ())
    .AddTraceSource ("TxRxMockChannel",
                     "Trace source indicating transmission of packet "
                     "from the MockChannel, used by the Animation "
                     "interface.",
                     MakeTraceSourceAccessor (&MockChannel::m_txrxMock),
                     "ns3::MockChannel::TxRxAnimationCallback")
  ;
  return tid;
}

//
// By default, you get a channel that
// has an "infitely" fast transmission speed and zero processing delay.
MockChannel::MockChannel() : Channel (), m_link (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

MockChannel::~MockChannel()
{
}

bool
MockChannel::Detach (uint32_t deviceId)
{
  NS_LOG_FUNCTION (this << deviceId);
  if (deviceId < m_link.size ())
  {
    if (!m_link[deviceId]->IsLinkUp ())
    {
      NS_LOG_WARN ("MockChannel::Detach(): Device is already detached (" << deviceId << ")");
      return false;
    }

    m_link[deviceId]->NotifyLinkDown ();
  }
  else
  {
    return false;
  }
  return true;
}

int32_t
MockChannel::Attach (Ptr<MockNetDevice> device)
{
  NS_LOG_FUNCTION (this << device);
  NS_ASSERT (device != 0);
  m_link.push_back(device);
  return  m_link.size() - 1;
}

std::size_t
MockChannel::GetNDevices (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_link.size ();
}

Ptr<NetDevice>
MockChannel::GetDevice (std::size_t i) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_link[i];
}

Time
MockChannel::GetDelay (Ptr<const MockNetDevice> src, Ptr<const MockNetDevice> dst, Time txTime) const
{
  NS_LOG_DEBUG ("Get delay from " << src << " to " << dst);

  Ptr<MobilityModel> modSrc = src->GetMobilityModel ();
  Ptr<MobilityModel> modDst = dst->GetMobilityModel ();

  Time propagationDelay = m_propagationDelay->GetDelay (modSrc, modDst);

  return txTime + propagationDelay;
}

// TODO optimize
Ptr<MockNetDevice>
MockChannel::GetDevice (Address &addr) const
{
  for (Ptr<MockNetDevice> dev : m_link)
  {
    if (dev->GetAddress () == addr)
    {
      return dev;
    }
  }

  return 0;
}

Ptr<PropagationDelayModel>
MockChannel::GetPropagationDelay () const
{
  return m_propagationDelay;
}

Ptr<PropagationLossModel>
MockChannel::GetPropagationLoss () const
{
  return m_propagationLoss;
}

} // namespace ns3
