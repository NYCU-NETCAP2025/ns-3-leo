/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/node-container.h"
#include "ns3/mobility-helper.h"
#include "ns3/test.h"
#include "ns3/integer.h"
#include "ns3/nstime.h"

#include "../model/leo-circular-orbit-mobility-model.h"

using namespace ns3;

class LeoOrbitSpeedTestCase : public TestCase
{
public:
  LeoOrbitSpeedTestCase () : TestCase ("Test speed for 0 altitude") {}
  virtual ~LeoOrbitSpeedTestCase () {}
private:
  virtual void DoRun (void)
  {
    Ptr<LeoCircularOrbitMobilityModel> mob = CreateObject<LeoCircularOrbitMobilityModel> ();
    mob->SetAttribute ("Altitude", DoubleValue (0.0));

    NS_TEST_ASSERT_MSG_EQ ((uint64_t) mob->GetSpeed (), (uint64_t) 7909.79, "Unexpected velocity at earths surface");
  }
};

class LeoOrbitPositionTestCase : public TestCase
{
public:
  LeoOrbitPositionTestCase () : TestCase ("Test position for 0 altitude and 1.0 inclination") {}
  virtual ~LeoOrbitPositionTestCase () {}
private:
  virtual void DoRun (void)
  {
    Ptr<LeoCircularOrbitMobilityModel> mob = CreateObject<LeoCircularOrbitMobilityModel> ();
    mob->SetAttribute ("Altitude", DoubleValue (0.0));
    mob->SetAttribute ("Inclination", DoubleValue (1.0));

    NS_TEST_ASSERT_MSG_EQ_TOL (mob->GetPosition ().GetLength(), LEO_EARTH_RAD_M, 0.1, "Unexpected position on earths surface for 1 deg inclination");
  }
};

class LeoOrbitProgressTestCase : public TestCase
{
public:
  LeoOrbitProgressTestCase () : TestCase ("Test position shoudl not be the same after some time") {}
  virtual ~LeoOrbitProgressTestCase () {}
private:
  void TestLengthPosition (double expl, double expx, Ptr<LeoCircularOrbitMobilityModel> mob)
  {
    Vector pos = mob->GetPosition ();
    NS_TEST_EXPECT_MSG_EQ_TOL_INTERNAL (pos.GetLength (), expl, 0.001, "Distance to earth should be the same", __FILE__, __LINE__);
    NS_TEST_EXPECT_MSG_NE_INTERNAL (pos.x, expx, "Position should not be equal", __FILE__, __LINE__);
  }

  virtual void DoRun (void)
  {
    Ptr<LeoCircularOrbitMobilityModel> mob = CreateObject<LeoCircularOrbitMobilityModel> ();
    mob->SetAttribute ("Altitude", DoubleValue (0.0));
    mob->SetAttribute ("Inclination", DoubleValue (20.0));

    Vector pos = mob->GetPosition ();
    Simulator::Schedule (Seconds (100.0), &LeoOrbitProgressTestCase::TestLengthPosition, this, LEO_EARTH_RAD_M, pos.x, mob);
    Simulator::Stop (Seconds (101.0));
    Simulator::Run ();
    Simulator::Destroy ();
  }
};

class LeoOrbitLatitudeTestCase : public TestCase
{
public:
  LeoOrbitLatitudeTestCase () : TestCase ("Test offset between neighboring satellites planes") {}
  virtual ~LeoOrbitLatitudeTestCase () {}
private:
  virtual void DoRun (void)
  {
    Ptr<LeoCircularOrbitMobilityModel> mob = CreateObject<LeoCircularOrbitMobilityModel> ();
    mob->SetAttribute ("Altitude", DoubleValue (1000.0));
    mob->SetAttribute ("Inclination", DoubleValue (20.0));

    Vector pos1 = mob->GetPosition ();

    mob->SetPosition (Vector3D (20.0, 0, 0));
    Vector pos2 = mob->GetPosition ();

    NS_TEST_ASSERT_MSG_NE (pos1.x, pos2.x, "Neighboring satellite should have different position");
  }
};

class LeoOrbitOffsetTestCase : public TestCase
{
public:
  LeoOrbitOffsetTestCase () : TestCase ("Test offset between neighboring satellites") {}
  virtual ~LeoOrbitOffsetTestCase () {}
private:
  virtual void DoRun (void)
  {
    Ptr<LeoCircularOrbitMobilityModel> mob = CreateObject<LeoCircularOrbitMobilityModel> ();
    mob->SetAttribute ("Altitude", DoubleValue (1000.0));
    mob->SetAttribute ("Inclination", DoubleValue (20.0));

    Vector pos1 = mob->GetPosition ();

    mob->SetPosition (Vector3D (0, 20.0, 0));
    Vector pos2 = mob->GetPosition ();

    NS_TEST_ASSERT_MSG_NE (pos1.x, pos2.x, "Neighboring satellite should have different position");
  }
};

class LeoOrbitTracingTestCase : public TestCase
{
public:
  LeoOrbitTracingTestCase () : TestCase ("Test tracing of position changes") {}
  virtual ~LeoOrbitTracingTestCase () {}
  static void CourseChange (std::string context, Ptr<const MobilityModel> position)
  {
    Vector pos = position->GetPosition ();
    std::cout << Simulator::Now () << ", pos=" << position << ", x=" << pos.x << ", y=" << pos.y
      << ", z=" << pos.z << std::endl;
  }
private:
  virtual void DoRun (void)
  {
    Ptr<LeoCircularOrbitMobilityModel> mob = CreateObject<LeoCircularOrbitMobilityModel> ();
    mob->SetAttribute ("Altitude", DoubleValue (1000.0));
    mob->SetAttribute ("Inclination", DoubleValue (20.0));
    mob->SetAttribute ("Precision", TimeValue (Seconds (10)));

    NodeContainer c;
    c.Create (10000);

    MobilityHelper mobility;
    mobility.SetPositionAllocator ("ns3::LeoCircularOrbitPostionAllocator",
                                   "NumOrbits", IntegerValue (100),
                                   "NumSatellites", IntegerValue (100));
    mobility.SetMobilityModel ("ns3::LeoCircularOrbitMobilityModel");
    mobility.Install (c);

    Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                     MakeCallback (&CourseChange));

    Simulator::Stop (Seconds (100.0));
    Simulator::Run ();
    Simulator::Destroy ();
  }
};


class LeoOrbitTestSuite : TestSuite
{
public:
  LeoOrbitTestSuite() : TestSuite ("leo-orbit", UNIT) {
      AddTestCase (new LeoOrbitSpeedTestCase, TestCase::QUICK);
      AddTestCase (new LeoOrbitPositionTestCase, TestCase::QUICK);
      AddTestCase (new LeoOrbitProgressTestCase, TestCase::QUICK);
      AddTestCase (new LeoOrbitLatitudeTestCase, TestCase::QUICK);
      AddTestCase (new LeoOrbitOffsetTestCase, TestCase::QUICK);
      AddTestCase (new LeoOrbitTracingTestCase, TestCase::EXTENSIVE);
  }
};

static LeoOrbitTestSuite leoOrbitTestSuite;
