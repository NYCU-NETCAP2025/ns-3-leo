/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <fstream>
#include <vector>

#include "ns3/log.h"
#include "ns3/config.h"
#include "ns3/waypoint.h"
#include "ns3/mobility-helper.h"
#include "ns3/double.h"
#include "ns3/integer.h"

#include "leo-orbit-node-helper.h"

using namespace std;

namespace ns3
{
NS_LOG_COMPONENT_DEFINE ("LeoOrbitNodeHelper");

LeoOrbitNodeHelper::LeoOrbitNodeHelper ()
{
  m_nodeFactory.SetTypeId ("ns3::Node");
}

LeoOrbitNodeHelper::~LeoOrbitNodeHelper ()
{
}

void
LeoOrbitNodeHelper::SetAttribute (string name, const AttributeValue &value)
{
  m_nodeFactory.Set (name, value);
}

NodeContainer
LeoOrbitNodeHelper::Install (const LeoOrbit &orbit)
{
  NS_LOG_FUNCTION (this << orbit);

  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::LeoCircularOrbitPostionAllocator",
                                 "NumOrbits", IntegerValue (orbit.planes),
                                 "NumSatellites", IntegerValue (orbit.sats));
  mobility.SetMobilityModel ("ns3::LeoCircularOrbitMobilityModel",
  			     "Altitude", DoubleValue (orbit.alt),
  			     "Inclination", DoubleValue (orbit.inc));

  NodeContainer c;
  c.Create (orbit.sats*orbit.planes);
  mobility.Install (c);

  return c;
}

NodeContainer
LeoOrbitNodeHelper::Install (const std::string &orbitFile)
{
  NS_LOG_FUNCTION (this << orbitFile);

  NodeContainer nodes;
  ifstream orbits;
  orbits.open (orbitFile, ifstream::in);
  LeoOrbit orbit;
  while ((orbits >> orbit))
    {
      nodes.Add (Install (orbit));
      NS_LOG_DEBUG ("Added orbit plane");
    }
  orbits.close ();

  NS_LOG_DEBUG ("Added " << nodes.GetN () << " nodes");

  return nodes;
}

NodeContainer
LeoOrbitNodeHelper::Install (const vector<LeoOrbit> &orbits)
{
  NS_LOG_FUNCTION (this << orbits);

  NodeContainer nodes;
  for (uint64_t i = 0; i < orbits.size(); i++)
    {
      nodes.Add (Install (orbits[i]));
      NS_LOG_DEBUG ("Added orbit plane");
    }

  NS_LOG_DEBUG ("Added " << nodes.GetN () << " nodes");

  return nodes;
}

}; // namespace ns3
