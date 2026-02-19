#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <optional>
#include <memory>
#include <stdexcept>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#endif /* __PROGTEST__ */

struct Function {
    std::string name;
    uint32_t start;
    uint32_t length;
    std::vector < std::pair < std::string, uint32_t > > calls;
};

struct ObjectFiles {
    std::map < std::string, uint32_t > exportFunctions;
    std::map < std::string, std::vector < uint32_t > > importFunctions;
    uint32_t machineCodePosition;
    uint32_t machineCodeSize;
    std::string fileName;
};

class CLinker
{
  public:
      CLinker () = default;
      ~CLinker () = default;

    CLinker & addFile ( const std::string & fileName )
    {
        std::ifstream fin ( fileName );
        if ( !fin ) throw std::runtime_error ( "Cannot open file: " + fileName );

        ObjectFiles obj;

        obj . fileName = fileName;
        uint32_t exportNum;
        uint32_t importNum;
        uint32_t machineCodeSize;

        fin . read ( reinterpret_cast < char * > ( & exportNum ), 4 );
        if ( !fin || fin . gcount () != 4 ) throw std::runtime_error ( "File error");
        fin . read ( reinterpret_cast < char * > ( & importNum ), 4 );
        if ( !fin || fin . gcount () != 4 ) throw std::runtime_error ( "File error");
        fin . read ( reinterpret_cast < char * > ( & machineCodeSize ), 4 );
        if ( !fin || fin . gcount () != 4 ) throw std::runtime_error ( "File error");

        for ( size_t i = 0; i < exportNum; i ++ ) {
            uint8_t nameSize;
            fin . read ( reinterpret_cast < char * > ( & nameSize ), 1 );
            if ( !fin || fin . gcount () != 1 ) throw std::runtime_error ( "File error");

            std::string funcName ( nameSize, 0 );
            fin . read ( funcName . data (), nameSize );
            if ( !fin || fin . gcount () != nameSize ) throw std::runtime_error ( "File error");


            uint32_t position;
            fin . read ( reinterpret_cast < char * > ( & position ), 4 );
            if ( !fin || fin . gcount () != 4 ) throw std::runtime_error ( "File error");

            if ( obj . exportFunctions . find ( funcName ) != obj . exportFunctions . end() ) 
                throw std::runtime_error ( "Duplicate function" );
            obj . exportFunctions [ funcName ] = position;
        }

        for ( size_t i = 0; i < importNum; i ++ ) {
            uint8_t nameSize;
            fin . read ( reinterpret_cast < char * > ( & nameSize ), 1 );
            if ( !fin || fin . gcount () != 1 ) throw std::runtime_error ( "File error");

            std::string funcName ( nameSize, 0 );
            fin . read ( funcName . data (), nameSize );
            if ( !fin || fin . gcount () != nameSize ) throw std::runtime_error ( "File error");

            uint32_t calledNum;
            fin . read ( reinterpret_cast < char * > ( & calledNum ), 4 );
            if ( !fin || fin . gcount () != 4 ) throw std::runtime_error ( "File error");

            std::vector < uint32_t > positions ( calledNum );
            fin . read ( reinterpret_cast < char * > ( positions . data () ), calledNum * 4 );
            if ( !fin || fin . gcount () != calledNum * 4 ) throw std::runtime_error ( "File error");

            std::sort ( positions . begin (), positions . end () );

            obj . importFunctions [ funcName ] = positions;
        }

        obj . machineCodePosition = fin . tellg ();
        obj . machineCodeSize = machineCodeSize;

        if ( !fin ) throw std::runtime_error ( "File problem: " + fileName );

        for ( const auto & [ name, position ] : obj . exportFunctions ) 
            allExports [ name ] = fileName;

        objects [ fileName ] = obj;

        return *this;
    }

    void linkOutput ( const std::string & fileName,
                      const std::string & entryPoint )
    {
        std::ofstream fout ( fileName );
        if ( !fout ) throw std::runtime_error ( "File problem: " + fileName );
        std::map < std::string, Function > result;

        graphCreate ( entryPoint, result );
        
        std::queue < std::string > q ( { entryPoint } );
        std::set < std::string > visited ( { entryPoint } );
        std::map < std::string, uint32_t > positions;
        uint32_t commonLength = result [ q . front () ] . length;
        while ( !q . empty () ) {
            Function func = result [ q . front () ];
            const ObjectFiles & obj = objects . at ( allExports . at ( q . front () ));
            q . pop ();
            std::vector < uint8_t > output ( func . length );
            uint32_t filePos = obj . machineCodePosition + func . start;
            std::ifstream fin ( obj . fileName );
            if ( !fin ) throw std::runtime_error ( "File problem: " + fileName );

            fin . seekg ( filePos, fin . beg );
            if ( !fin ) throw std::runtime_error ( "File problem: " + fileName );
            fin . read ( reinterpret_cast < char * > ( output . data () ), func . length ); 
            if ( !fin ) throw std::runtime_error ( "File problem: " + fileName );
            for ( const auto & [ name, pos ] : func . calls ) {
                auto res = visited . insert ( name ); 
                if ( res . second ) {
                    q . push ( name );
                    positions [ name ] = commonLength;
                    std::memcpy ( output . data () + pos - func . start, & commonLength, 4 );
                    commonLength += result [ name ] . length;
                } else {
                    std::memcpy ( output . data () + pos - func . start, & positions [ name ], 4 );
                } 
            }
            fout . write ( reinterpret_cast < char * > ( output . data () ), func . length );
            if ( !fout ) throw std::runtime_error ( "File problem: " + fileName );
        }

    }

  private: 
    std::map < std::string, std::string > allExports; // [ func, file ]
    std::map < std::string, ObjectFiles > objects; // [ file, objInfo ]

    void graphCreate (  const std::string & entryPoint, std::map < std::string, Function > & result ) {
        std::queue < std::string > q;
        std::set < std::string > visited;
        
        q . push ( entryPoint );
        while ( !q . empty () ) {
            const auto name = q . front ();
            q . pop ();
            if ( allExports . find ( name ) == allExports . end () ) 
                throw std::runtime_error ( "Missing export function: " );

            const ObjectFiles & obj = objects . at ( allExports . at ( name ) );

            Function func;
            try {
                func . start = obj . exportFunctions . at ( name );
            } catch ( ... ) {
                throw std::runtime_error ( "Missing import function in export: " + func . name );
            }
            uint32_t end = obj . machineCodeSize;
            if ( func . start > end ) 
                throw std::runtime_error ( "Out of machine code size" + func . name );

            for ( const auto & [ name, pos ] : obj . exportFunctions ) {
                if ( pos > func . start && end > pos ) end = pos;
            }
            func . length = end - func . start;


            for ( const auto& [ impName, positions] : obj . importFunctions ) {
                for ( auto pos : positions ) {
                    if ( pos >= func . start && pos < end ) {
                        func . calls . push_back ( { impName, pos } );
                        const auto & result = visited . insert ( impName ); 
                        if ( result . second ) q . push ( impName );
                    }
                }
            }
            result [ name ] = func ;
        }
    }
};

#ifndef __PROGTEST__
int main ()
{
  CLinker () . addFile ( "0in0.o" ) . linkOutput ( "0out", "strlen" );

  CLinker () . addFile ( "1in0.o" ) . linkOutput ( "1out", "main" );

  CLinker () . addFile ( "2in0.o" ) . addFile ( "2in1.o" ) . linkOutput ( "2out", "main" );

  CLinker () . addFile ( "3in0.o" ) . addFile ( "3in1.o" ) . linkOutput ( "3out", "towersOfHanoi" );

  try
  {
    CLinker () . addFile ( "4in0.o" ) . addFile ( "4in1.o" ) . linkOutput ( "4out", "unusedFunc" );
    assert ( "missing an exception" == nullptr );
  }
  catch ( const std::runtime_error & e )
  {
    // e . what (): Undefined symbol qsort
  }
  catch ( ... )
  {
    assert ( "invalid exception" == nullptr );
  }

  try
  {
    CLinker () . addFile ( "5in0.o" ) . linkOutput ( "5out", "main" );
    assert ( "missing an exception" == nullptr );
  }
  catch ( const std::runtime_error & e )
  {
    // e . what (): Duplicate symbol: printf
  }
  catch ( ... )
  {
    assert ( "invalid exception" == nullptr );
  }

  try
  {
    CLinker () . addFile ( "6in0.o" ) . linkOutput ( "6out", "strlen" );
    assert ( "missing an exception" == nullptr );
  }
  catch ( const std::runtime_error & e )
  {
    // e . what (): Cannot read input file
  }
  catch ( ... )
  {
    assert ( "invalid exception" == nullptr );
  }

  try
  {
    CLinker () . addFile ( "7in0.o" ) . linkOutput ( "7out", "strlen2" );
    assert ( "missing an exception" == nullptr );
  }
  catch ( const std::runtime_error & e )
  {
    // e . what (): Undefined symbol strlen2
  }
  catch ( ... )
  {
    assert ( "invalid exception" == nullptr );
  }

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
