/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/leo-module.h"
#include "ns3/network-module.h"
#include "ns3/aodv-module.h"
#include "ns3/udp-server.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LeoDelayTracingExample");

static void
EchoRx (std::string context, Ptr<const Packet> packet)
{
  SeqTsHeader seqTs;
  Ptr<Packet> p = packet->Copy ();
  p->RemoveHeader (seqTs);
  // seqnr, timestamp, delay
  std::cout << context << "," << seqTs.GetSeq () << "," << seqTs.GetTs () << "," << Simulator::Now () - seqTs.GetTs () << std::endl;
}

int main (int argc, char *argv[])
{
  NodeContainer satellites;
  for (double incl: { 70.0, 100.0 })
    {
      NodeContainer c;
      c.Create (10*10);

      MobilityHelper mobility;
      mobility.SetPositionAllocator ("ns3::LeoCircularOrbitPostionAllocator",
                                     "NumOrbits", IntegerValue (10),
                                     "NumSatellites", IntegerValue (10));
      mobility.SetMobilityModel ("ns3::LeoCircularOrbitMobilityModel",
  			     	 "Altitude", DoubleValue (1200.0),
  			     	 "Inclination", DoubleValue (incl),
  			     	 "Precision", TimeValue (Minutes (1)));
      mobility.Install (c);
      satellites.Add (c);
    }

  LeoGndNodeHelper ground;
  NodeContainer stations = ground.Install ("contrib/leo/data/ground-stations/airports-60.waypoints");

  NetDeviceContainer islNet, utNet;

  IslHelper islCh;
  islCh.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  islCh.SetDeviceAttribute ("ReceiveErrorModel", StringValue ("ns3::BurstErrorModel"));
  islCh.SetChannelAttribute ("PropagationDelay", StringValue ("ns3::ConstantSpeedPropagationDelayModel"));
  islCh.SetChannelAttribute ("PropagationLoss", StringValue ("ns3::IslPropagationLossModel"));
  islNet = islCh.Install (satellites);

  LeoChannelHelper utCh;
  utCh.SetConstellation ("TelesatUser");
  utNet = utCh.Install (satellites, stations);

  // Install internet stack on nodes
  AodvHelper aodv;
  aodv.Set ("HelloInterval", TimeValue (Seconds (10)));
  aodv.Set ("TtlStart", UintegerValue (10));
  aodv.Set ("TtlIncrement", UintegerValue (10));
  aodv.Set ("TtlThreshold", UintegerValue (1000));
  aodv.Set ("RreqRetries", UintegerValue (100));
  aodv.Set ("RreqRateLimit", UintegerValue (100));
  aodv.Set ("RerrRateLimit", UintegerValue (100));
  aodv.Set ("ActiveRouteTimeout", TimeValue (Seconds (10)));
  aodv.Set ("NextHopWait", TimeValue (MilliSeconds (100)));
  aodv.Set ("NetDiameter", UintegerValue (1000));
  aodv.Set ("PathDiscoveryTime", TimeValue (Seconds (1)));
  InternetStackHelper stack;
  stack.SetRoutingHelper (aodv);
  stack.Install (satellites);
  stack.Install (stations);

  // Make all networks addressable for legacy protocol
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.0.0", "255.255.0.0");
  Ipv4InterfaceContainer islIp = ipv4.Assign (islNet);
  ipv4.SetBase ("10.3.0.0", "255.255.0.0");
  Ipv4InterfaceContainer utIp = ipv4.Assign (utNet);

  // we want to ping terminals
  UdpServerHelper echoServer (9);
  ApplicationContainer serverApps = echoServer.Install (stations.Get (1));

  // install a client on one of the terminals
  ApplicationContainer clientApps;
  Address remote = stations.Get (1)->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal ();//utIp.GetAddress (1, 0);
  UdpClientHelper echoClient (remote, 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (360));
  echoClient.SetAttribute ("Interval", TimeValue (Minutes (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  clientApps.Add (echoClient.Install (stations.Get (3)));

  Config::Connect ("/NodeList/*/ApplicationList/*/$ns3::UdpServer/Rx",
  		   MakeCallback (&EchoRx));

  std::cout << "Context,Sequence Number,Timestamp,Delay" << std::endl;

  serverApps.Start (Seconds (1));
  clientApps.Start (Seconds (2));

  Simulator::Stop (Minutes (60));
  Simulator::Run ();
  Simulator::Destroy ();

  Ptr<UdpServer> server = StaticCast<UdpServer> (serverApps.Get (0));
  std::cout << "Received,Lost" << std::endl
    << server->GetReceived () << "," << server->GetLost () << std::endl;

  return 0;
}
