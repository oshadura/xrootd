//------------------------------------------------------------------------------
// Copyright (c) 2011 by European Organization for Nuclear Research (CERN)
// Author: Lukasz Janyst <ljanyst@cern.ch>
// See the LICENCE file for details.
//------------------------------------------------------------------------------

#include <cppunit/extensions/HelperMacros.h>
#include <XrdCl/XrdClQuery.hh>

#include <pthread.h>

#include "TestEnv.hh"

//------------------------------------------------------------------------------
// Declaration
//------------------------------------------------------------------------------
class QueryTest: public CppUnit::TestCase
{
  public:
    CPPUNIT_TEST_SUITE( QueryTest );
      CPPUNIT_TEST( LocateTest );
      CPPUNIT_TEST( MvTest );
      CPPUNIT_TEST( ServerQueryTest );
      CPPUNIT_TEST( TruncateRmTest );
      CPPUNIT_TEST( MkdirRmdirTest );
      CPPUNIT_TEST( ChmodTest );
      CPPUNIT_TEST( PingTest );
      CPPUNIT_TEST( StatTest );
      CPPUNIT_TEST( ProtocolTest );
      CPPUNIT_TEST( DeepLocateTest );
      CPPUNIT_TEST( DirListTest );
    CPPUNIT_TEST_SUITE_END();
    void LocateTest();
    void MvTest();
    void ServerQueryTest();
    void TruncateRmTest();
    void MkdirRmdirTest();
    void ChmodTest();
    void PingTest();
    void StatTest();
    void ProtocolTest();
    void DeepLocateTest();
    void DirListTest();
};

CPPUNIT_TEST_SUITE_REGISTRATION( QueryTest );

//------------------------------------------------------------------------------
// Locate test
//------------------------------------------------------------------------------
void QueryTest::LocateTest()
{
  using namespace XrdClient;

  //----------------------------------------------------------------------------
  // Get the environment variables
  //----------------------------------------------------------------------------
  Env *testEnv = TestEnv::GetEnv();

  std::string address;
  std::string dataPath;

  CPPUNIT_ASSERT( testEnv->GetString( "MainServerURL", address ) );
  CPPUNIT_ASSERT( testEnv->GetString( "DataPath", dataPath ) );

  URL url( address );
  CPPUNIT_ASSERT( url.IsValid() );

  std::string filePath = dataPath + "/89120cec-5244-444c-9313-703e4bee72de.dat";

  //----------------------------------------------------------------------------
  // Query the server for all of the file locations
  //----------------------------------------------------------------------------
  Query query( url );

  LocationInfo *locations = 0;
  XRootDStatus st = query.Locate( filePath, OpenFlags::Refresh, locations );
  CPPUNIT_ASSERT( st.IsOK() );
  CPPUNIT_ASSERT( locations );
  CPPUNIT_ASSERT( locations->GetSize() != 0 );
  delete locations;
}

//------------------------------------------------------------------------------
// Mv test
//------------------------------------------------------------------------------
void QueryTest::MvTest()
{
  using namespace XrdClient;

  //----------------------------------------------------------------------------
  // Get the environment variables
  //----------------------------------------------------------------------------
  Env *testEnv = TestEnv::GetEnv();

  std::string address;
  std::string dataPath;

  CPPUNIT_ASSERT( testEnv->GetString( "DiskServerURL", address ) );
  CPPUNIT_ASSERT( testEnv->GetString( "DataPath", dataPath ) );

  URL url( address );
  CPPUNIT_ASSERT( url.IsValid() );

  std::string filePath1 = dataPath + "/89120cec-5244-444c-9313-703e4bee72de.dat";
  std::string filePath2 = dataPath + "/89120cec-5244-444c-9313-703e4bee72de.dat2";

  Query query( url );

  XRootDStatus st = query.Mv( filePath1, filePath2 );
  CPPUNIT_ASSERT( st.IsOK() );
  st = query.Mv( filePath2, filePath1 );
  CPPUNIT_ASSERT( st.IsOK() );
}

//------------------------------------------------------------------------------
// Query test
//------------------------------------------------------------------------------
void QueryTest::ServerQueryTest()
{
  using namespace XrdClient;

  //----------------------------------------------------------------------------
  // Get the environment variables
  //----------------------------------------------------------------------------
  Env *testEnv = TestEnv::GetEnv();

  std::string address;
  std::string dataPath;

  CPPUNIT_ASSERT( testEnv->GetString( "MainServerURL", address ) );
  CPPUNIT_ASSERT( testEnv->GetString( "DataPath", dataPath ) );

  URL url( address );
  CPPUNIT_ASSERT( url.IsValid() );

  std::string filePath = dataPath + "/89120cec-5244-444c-9313-703e4bee72de.dat";

  Query query( url );
  Buffer *response = 0;
  Buffer  arg;
  arg.FromString( filePath );
  XRootDStatus st = query.ServerQuery( QueryCode::Checksum, arg, response );
  CPPUNIT_ASSERT( st.IsOK() );
  CPPUNIT_ASSERT( response );
  CPPUNIT_ASSERT( response->GetSize() != 0 );
  delete response;
}

//------------------------------------------------------------------------------
// Truncate/Rm test
//------------------------------------------------------------------------------
void QueryTest::TruncateRmTest()
{
  using namespace XrdClient;

  //----------------------------------------------------------------------------
  // Get the environment variables
  //----------------------------------------------------------------------------
  Env *testEnv = TestEnv::GetEnv();

  std::string address;
  std::string dataPath;

  CPPUNIT_ASSERT( testEnv->GetString( "DiskServerURL", address ) );
  CPPUNIT_ASSERT( testEnv->GetString( "DataPath", dataPath ) );

  URL url( address );
  CPPUNIT_ASSERT( url.IsValid() );

  std::string filePath = dataPath + "/testfile";

  Query query( url );

  XRootDStatus st = query.Truncate( filePath, 10000000 );
  CPPUNIT_ASSERT( st.IsOK() );
  st = query.Rm( filePath );
  CPPUNIT_ASSERT( st.IsOK() );
}

//------------------------------------------------------------------------------
// Mkdir/Rmdir test
//------------------------------------------------------------------------------
void QueryTest::MkdirRmdirTest()
{
  using namespace XrdClient;

  //----------------------------------------------------------------------------
  // Get the environment variables
  //----------------------------------------------------------------------------
  Env *testEnv = TestEnv::GetEnv();

  std::string address;
  std::string dataPath;

  CPPUNIT_ASSERT( testEnv->GetString( "DiskServerURL", address ) );
  CPPUNIT_ASSERT( testEnv->GetString( "DataPath", dataPath ) );

  URL url( address );
  CPPUNIT_ASSERT( url.IsValid() );

  std::string dirPath1 = dataPath + "/testdir";
  std::string dirPath2 = dataPath + "/testdir/asdads";

  Query query( url );

  XRootDStatus st = query.MkDir( dirPath2, MkDirFlags::MakePath,
                          AccessMode::UR | AccessMode::UW | AccessMode::UX );
  CPPUNIT_ASSERT( st.IsOK() );
  st = query.RmDir( dirPath2 );
  CPPUNIT_ASSERT( st.IsOK() );
  st = query.RmDir( dirPath1 );
  CPPUNIT_ASSERT( st.IsOK() );
}

//------------------------------------------------------------------------------
// Chmod test
//------------------------------------------------------------------------------
void QueryTest::ChmodTest()
{
  using namespace XrdClient;

  //----------------------------------------------------------------------------
  // Get the environment variables
  //----------------------------------------------------------------------------
  Env *testEnv = TestEnv::GetEnv();

  std::string address;
  std::string dataPath;

  CPPUNIT_ASSERT( testEnv->GetString( "DiskServerURL", address ) );
  CPPUNIT_ASSERT( testEnv->GetString( "DataPath", dataPath ) );

  URL url( address );
  CPPUNIT_ASSERT( url.IsValid() );

  std::string dirPath = dataPath + "/testdir";

  Query query( url );

  XRootDStatus st = query.MkDir( dirPath, MkDirFlags::MakePath,
                          AccessMode::UR | AccessMode::UW | AccessMode::UX );
  CPPUNIT_ASSERT( st.IsOK() );
  st = query.ChMod( dirPath,
                    AccessMode::UR | AccessMode::UW | AccessMode::UX |
                    AccessMode::GR | AccessMode::GX );
  CPPUNIT_ASSERT( st.IsOK() );
  st = query.RmDir( dirPath );
  CPPUNIT_ASSERT( st.IsOK() );
}

//------------------------------------------------------------------------------
// Locate test
//------------------------------------------------------------------------------
void QueryTest::PingTest()
{
  using namespace XrdClient;

  //----------------------------------------------------------------------------
  // Get the environment variables
  //----------------------------------------------------------------------------
  Env *testEnv = TestEnv::GetEnv();

  std::string address;
  CPPUNIT_ASSERT( testEnv->GetString( "MainServerURL", address ) );
  URL url( address );
  CPPUNIT_ASSERT( url.IsValid() );

  Query query( url );
  XRootDStatus st = query.Ping();
  CPPUNIT_ASSERT( st.IsOK() );
}

//------------------------------------------------------------------------------
// Stat test
//------------------------------------------------------------------------------
void QueryTest::StatTest()
{
  using namespace XrdClient;

  Env *testEnv = TestEnv::GetEnv();

  std::string address;
  std::string dataPath;

  CPPUNIT_ASSERT( testEnv->GetString( "MainServerURL", address ) );
  CPPUNIT_ASSERT( testEnv->GetString( "DataPath", dataPath ) );

  URL url( address );
  CPPUNIT_ASSERT( url.IsValid() );

  std::string filePath = dataPath + "/89120cec-5244-444c-9313-703e4bee72de.dat";

  Query query( url );
  StatInfo *response = 0;
  XRootDStatus st = query.Stat( filePath, StatFlags::Object, response );
  CPPUNIT_ASSERT( st.IsOK() );
  CPPUNIT_ASSERT( response );
  CPPUNIT_ASSERT( response->GetType() == StatInfo::Object );
  CPPUNIT_ASSERT( response->GetSize() == 1048576000 );
  CPPUNIT_ASSERT( response->TestFlags( StatInfo::Readable ) );
  CPPUNIT_ASSERT( response->TestFlags( StatInfo::Writable ) );
  CPPUNIT_ASSERT( !response->TestFlags( StatInfo::IsDir ) );
  delete response;
}

//------------------------------------------------------------------------------
// Protocol test
//------------------------------------------------------------------------------
void QueryTest::ProtocolTest()
{
  using namespace XrdClient;

  Env *testEnv = TestEnv::GetEnv();

  std::string address;
  CPPUNIT_ASSERT( testEnv->GetString( "MainServerURL", address ) );
  URL url( address );
  CPPUNIT_ASSERT( url.IsValid() );

  Query query( url );
  ProtocolInfo *response = 0;
  XRootDStatus st = query.Protocol( response );
  CPPUNIT_ASSERT( st.IsOK() );
  CPPUNIT_ASSERT( response );
  delete response;
}

//------------------------------------------------------------------------------
// Deep locate test
//------------------------------------------------------------------------------
void QueryTest::DeepLocateTest()
{
  using namespace XrdClient;

  //----------------------------------------------------------------------------
  // Get the environment variables
  //----------------------------------------------------------------------------
  Env *testEnv = TestEnv::GetEnv();

  std::string address;
  std::string dataPath;

  CPPUNIT_ASSERT( testEnv->GetString( "MainServerURL", address ) );
  CPPUNIT_ASSERT( testEnv->GetString( "DataPath", dataPath ) );

  URL url( address );
  CPPUNIT_ASSERT( url.IsValid() );

  std::string filePath = dataPath + "/89120cec-5244-444c-9313-703e4bee72de.dat";

  //----------------------------------------------------------------------------
  // Query the server for all of the file locations
  //----------------------------------------------------------------------------
  Query query( url );

  LocationInfo *locations = 0;
  XRootDStatus st = query.DeepLocate( filePath, OpenFlags::Refresh, locations );
  CPPUNIT_ASSERT( st.IsOK() );
  CPPUNIT_ASSERT( locations );
  CPPUNIT_ASSERT( locations->GetSize() != 0 );
  LocationInfo::LocationIterator it = locations->Begin();
  for( ; it != locations->End(); ++it )
    CPPUNIT_ASSERT( it->IsServer() );
  delete locations;
}

//------------------------------------------------------------------------------
// Dir list
//------------------------------------------------------------------------------
void QueryTest::DirListTest()
{
  using namespace XrdClient;

  //----------------------------------------------------------------------------
  // Get the environment variables
  //----------------------------------------------------------------------------
  Env *testEnv = TestEnv::GetEnv();

  std::string address;
  std::string dataPath;

  CPPUNIT_ASSERT( testEnv->GetString( "MainServerURL", address ) );
  CPPUNIT_ASSERT( testEnv->GetString( "DataPath", dataPath ) );

  URL url( address );
  CPPUNIT_ASSERT( url.IsValid() );

  std::string lsPath = dataPath + "/bigdir";

  //----------------------------------------------------------------------------
  // Query the server for all of the file locations
  //----------------------------------------------------------------------------
  Query query( url );

  DirectoryList *list = 0;
  XRootDStatus st = query.DirList( lsPath, DirListFlags::None, list );
  CPPUNIT_ASSERT( st.IsOK() );
  CPPUNIT_ASSERT( list );
  CPPUNIT_ASSERT( list->GetSize() == 10000 );
  delete list;
}
