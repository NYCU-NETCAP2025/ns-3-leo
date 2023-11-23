/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/node-container.h"

#include "ns3/leo-module.h"
#include "ns3/test.h"

using namespace ns3;

class IslIcmpTestCase : public TestCase
{
public:
  IslIcmpTestCase ();
  virtual ~IslIcmpTestCase ();

private:
  virtual void DoRun (void);
};

IslIcmpTestCase::IslIcmpTestCase ()
  : TestCase ("Test connectivity using ICMP")
{
}

IslIcmpTestCase::~IslIcmpTestCase ()
{
  Simulator::Destroy ();
}

void
IslIcmpTestCase::DoRun (void)
{
  NodeContainer nodes;
  nodes.Create (3);

  IslHelper isl;
  isl.SetDeviceAttribute ("DataRate", StringValue ("5Gbps"));
  isl.SetChannelAttribute ("PropagationDelay", StringValue ("ns3::ConstantSpeedPropagationDelayModel"));
  isl.SetChannelAttribute ("PropagationLoss", StringValue ("ns3::IslPropagationLossModel"));
  isl.SetDeviceAttribute ("MobilityModel", StringValue ("ns3::LeoMobilityModel"));

  NetDeviceContainer devices;
  devices = isl.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv6AddressHelper address;

  Ipv6InterfaceContainer interfaces = address.Assign (devices);

  NdCacheHelper nsHelper;
  nsHelper.Install (devices, interfaces);

  // install echo server on all nodes
  UdpEchoServerHelper echoServer (9);
  ApplicationContainer serverApps = echoServer.Install (nodes);

  ApplicationContainer clientApps;
  UdpEchoClientHelper echoClient (devices.Get (0)->GetAddress (), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (10));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  for (uint32_t i = 1; i < nodes.GetN (); i++)
    {
      Address destAddress = interfaces.GetAddress (i, 0);
      echoClient.SetAttribute ("RemoteAddress", AddressValue (destAddress));

      clientApps.Add (echoClient.Install (nodes.Get (0)));
    }

  serverApps.Start (Seconds (1.0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));
  serverApps.Stop (Seconds (10.0));

  Simulator::Run ();
}

class IslTestSuite : public TestSuite
{
public:
  IslTestSuite ();
};

IslTestSuite::IslTestSuite ()
  : TestSuite ("isl", EXAMPLE)
{
  // TestDuration for TestCase can be QUICK, EXTENSIVE or TAKES_FOREVER
  AddTestCase (new IslIcmpTestCase, TestCase::EXTENSIVE);
}

// Do not forget to allocate an instance of this TestSuite
static IslTestSuite leoTestSuite;
