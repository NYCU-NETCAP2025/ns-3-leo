/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/node-container.h"

#include "ns3/leo-module.h"
#include "ns3/test.h"

using namespace ns3;

class LeoMockChannelTransmitUnknownTestCase : public TestCase
{
public:
  LeoMockChannelTransmitUnknownTestCase () : TestCase ("transmission from unknown source fails") {}
  virtual ~LeoMockChannelTransmitUnknownTestCase () {}

private:
  virtual void DoRun (void)
  {
    Ptr<LeoMockChannel> channel = CreateObject<LeoMockChannel> ();
    Packet *packet = new Packet ();
    Ptr<Packet> p = Ptr<Packet>(packet);
    Address destAddr;
    Time txTime;
    channel->SetAttribute ("PropagationDelay", StringValue ("ns3::ConstantSpeedPropagationDelayModel"));
    channel->SetAttribute ("PropagationLoss", StringValue ("ns3::LeoPropagationLossModel"));
    bool result = channel->TransmitStart (p, 10000, destAddr, txTime);

    NS_TEST_ASSERT_MSG_EQ (result, false, "unknown source fails to deliver");
  }
};

class LeoMockChannelTransmitKnownTestCase : public TestCase
{
public:
  LeoMockChannelTransmitKnownTestCase () : TestCase ("transmission from known source succeeds") {}
  virtual ~LeoMockChannelTransmitKnownTestCase () {}
private:
  virtual void DoRun (void)
  {
    Ptr<LeoMockChannel> channel = CreateObject<LeoMockChannel> ();
    channel->SetAttribute ("PropagationDelay", StringValue ("ns3::ConstantSpeedPropagationDelayModel"));
    channel->SetAttribute ("PropagationLoss", StringValue ("ns3::LeoPropagationLossModel"));

    Packet *packet = new Packet ();
    Ptr<Packet> p = Ptr<Packet>(packet);

    Ptr<Node> srcNode = CreateObject<Node> ();
    Ptr<LeoMockNetDevice> srcDev = CreateObject<LeoMockNetDevice> ();
    srcDev->SetNode (srcNode);
    srcDev->SetDeviceType (LeoMockNetDevice::GND);
    srcDev->SetAddress (Mac48Address::Allocate ());
    int32_t srcId = channel->Attach (srcDev);

    Ptr<Node> dstNode = CreateObject<Node> ();
    Ptr<LeoMockNetDevice> dstDev = CreateObject<LeoMockNetDevice> ();
    dstDev->SetNode (dstNode);
    dstDev->SetDeviceType (LeoMockNetDevice::SAT);
    dstDev->SetAddress (Mac48Address::Allocate ());
    channel->Attach (dstDev);

    Address destAddr = dstDev->GetAddress ();
    Time txTime;
    bool result = channel->TransmitStart (p, srcId, destAddr, txTime);

    NS_TEST_ASSERT_MSG_EQ (result, true, "known source does not deliver");
  }
};

class LeoMockChannelTransmitSpaceGroundTestCase : public TestCase
{
public:
  LeoMockChannelTransmitSpaceGroundTestCase () : TestCase ("space to ground transmission succeeds") {}
  virtual ~LeoMockChannelTransmitSpaceGroundTestCase () {}
private:
  virtual void DoRun (void)
  {
    Ptr<LeoMockChannel> channel = CreateObject<LeoMockChannel> ();
    channel->SetAttribute ("PropagationDelay", StringValue ("ns3::ConstantSpeedPropagationDelayModel"));
    channel->SetAttribute ("PropagationLoss", StringValue ("ns3::LeoPropagationLossModel"));

    Packet *packet = new Packet ();
    Ptr<Packet> p = Ptr<Packet>(packet);

    Ptr<Node> srcNode = CreateObject<Node> ();
    Ptr<LeoMockNetDevice> srcDev = CreateObject<LeoMockNetDevice> ();
    srcDev->SetNode (srcNode);
    srcDev->SetDeviceType (LeoMockNetDevice::SAT);
    srcDev->SetAddress (Mac48Address::Allocate ());
    int32_t srcId = channel->Attach (srcDev);

    Ptr<Node> dstNode = CreateObject<Node> ();
    Ptr<LeoMockNetDevice> dstDev = CreateObject<LeoMockNetDevice> ();
    dstDev->SetNode (dstNode);
    dstDev->SetDeviceType (LeoMockNetDevice::GND);
    dstDev->SetAddress (Mac48Address::Allocate ());
    channel->Attach (dstDev);

    Address destAddr = dstDev->GetAddress ();
    Time txTime;
    bool result = channel->TransmitStart (p, srcId, destAddr, txTime);

    NS_TEST_ASSERT_MSG_EQ (result, true, "space to ground transmission failed");
  }
};

class LeoMockChannelTestSuite : public TestSuite
{
public:
  LeoMockChannelTestSuite ();
};

LeoMockChannelTestSuite::LeoMockChannelTestSuite ()
  : TestSuite ("leo-mock-channel", UNIT)
{
  AddTestCase (new LeoMockChannelTransmitUnknownTestCase, TestCase::QUICK);
  AddTestCase (new LeoMockChannelTransmitKnownTestCase, TestCase::QUICK);
  AddTestCase (new LeoMockChannelTransmitSpaceGroundTestCase, TestCase::QUICK);
}

static LeoMockChannelTestSuite islMockChannelTestSuite;
