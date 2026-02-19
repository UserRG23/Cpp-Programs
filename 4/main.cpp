#include <ios>
#include <ostream>
#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <compare>
#include <functional>
#include <optional>

class CTimeStamp
{
  public:
    CTimeStamp    ( int               year,
                    int               month,
                    int               day,
                    int               hour,
                    int               minute,
                    double            sec ) :
            day ( day ),
            month ( month ),
            year ( year ),
            hour ( hour ),
            minute ( minute ),
            sec ( sec ) {
                milisec = sec * 10000;
                milisec = ( milisec % 10 ) == 0 ? ( milisec % 10000 / 10 ) : ( milisec % 10000 / 10 + 1 ); 
            }

    int                   compare       ( const CTimeStamp & x ) const
    {
        const int first [] = { year, month, day, hour, minute, sec, milisec };
        const int second [] = { x.year, x.month, x.day, x.hour, x.minute, x.sec, x.milisec };
        for ( int pos = 0; pos < 7; pos ++ ) {
            int result = first [ pos ] - second [ pos ];
            if ( result != 0 ) return result;
        }
        return 0;
    }

    friend std::ostream & operator <<   ( std::ostream     & os,
                                          const CTimeStamp & x )
    {
        os << x.year << '-' << std::setw ( 2 ) << std::setfill ( '0' ) << x.month << '-'
                            << std::setw ( 2 ) << std::setfill ( '0' ) << x.day << ' '
                            << std::setw ( 2 ) << std::setfill ( '0' ) << x.hour << ':'
                            << std::setw ( 2 ) << std::setfill ( '0' ) << x.minute << ':'
                            << std::setw ( 2 ) << std::setfill ( '0' ) << x.sec << '.'
                            << std::setw ( 3 ) << std::setfill ( '0' ) << x.milisec; 
        return os;
    }
  private:
    int day = 0, month = 0, year = 0, hour = 0, minute = 0;
    int sec = 0, milisec = 0;
};

class CMail
{
  public:
    CMail         ( const CTimeStamp & timeStamp,
                    const std::string & from,
                    const std::string & to,
                    const std::optional<std::string> & subject ) :
           m_to ( to ),
           m_time ( timeStamp ),
           m_from ( from ),
           m_subject ( subject ) {}

    CMail         ( const CTimeStamp & timeStamp,
                    const std::string & from ) :
        m_time ( timeStamp ),
        m_from ( from ) {}

    int                   compareByTime ( const CTimeStamp & x ) const { return m_time.compare ( x ); }
    int                   compareByTime ( const CMail      & x ) const { return m_time.compare ( x.m_time ); }

    const std::string   & from          () const { return m_from; }
    const std::string   & to            () const { return m_to; }
    const std::optional<std::string>  & subject       () const { return m_subject; }
    const CTimeStamp    & timeStamp     () const { return m_time; }

    friend std::ostream & operator <<   ( std::ostream     & os,
                                          const CMail      & x )
    {
        if ( x.m_subject )
            os << x.m_time << ' ' << x.m_from << " -> " << x.m_to << ", subject: " << x.m_subject.value();
        else
            os << x.m_time << ' ' << x.m_from << " -> " << x.m_to;
        return os;
    }
  private:
    std::string m_to;
    CTimeStamp m_time;
    std::string m_from;
    std::optional < std::string > m_subject;
};
bool compare_by_time ( const CMail & first, const CTimeStamp & second ) {
    return second.compare ( first.timeStamp() ) < 0;
}
// your code will be compiled in a separate namespace
namespace MysteriousNamespace {
#endif /* __PROGTEST__ */
//----------------------------------------------------------------------------------------
class CMailLog
{
  public:
      int                   parseLog      ( std::istream     & in ) {
          int reconstructed = 0;
          std::map < std::string, std::pair < std::string, std::optional < std::string > > > mailData;
          while ( in ) {
              std::string monthStr, mailId, garbage, logType;
              int day, month, year, hour, minute;
              double sec;
              char separator;

              if ( !( in >> monthStr >> day >> year >> hour >> separator >> minute >> separator >> sec ) ) continue;
              if ( !( in >> garbage >> mailId ) ) continue;
              if ( mailId.back() != ':' ) continue;
              month = monthToNum ( monthStr );
              if ( month == 13 ) continue;

              CTimeStamp time ( year, month, day, hour, minute, sec );
              getline ( in, logType );

              if ( logType.compare( 1, 5, "from=" ) == 0 ) {
                  mailData [ mailId ] . first = logType.substr ( 6 );
              }
              if ( logType.compare( 1, 8, "subject=" ) == 0 ) {
                  if ( mailData.find ( mailId ) != mailData.end() )
                      mailData [ mailId ] . second = logType.substr ( 9 );
              }
              if ( logType.compare( 1, 3, "to=" ) == 0 ) {
                  if ( mailData.find ( mailId ) != mailData.end() ) {
                      auto [ from, subject ] = mailData.at ( mailId );
                      m_data.push_back ( CMail ( time, from, logType.substr ( 4 ), subject ) );
                      reconstructed ++;
                  }
              }
          }
          std::sort ( m_data.begin(), m_data.end(), [] ( const CMail & first, const CMail & second ) {
                                                            if ( first.compareByTime ( second ) < 0 ) return true;
                                                            return false;
                                                            } );
          return reconstructed;
      }

    std::list<CMail>      listMail      ( const CTimeStamp & from,
                                          const CTimeStamp & to ) const {
        size_t start;
        std::list < CMail > result;
        if ( !findBound ( from, start ) ) return result;
        for ( size_t it = start; it != m_data.size() && m_data.at ( it ) . compareByTime ( to ) <= 0; ++ it ) {
            result.push_back ( m_data.at ( it ) );
        }
        return result;
    }

    std::set<std::string> activeUsers   ( const CTimeStamp & from,
                                          const CTimeStamp & to ) const {
        size_t start;
        std::set < std::string >  result;
        if ( !findBound ( from, start ) ) return result;
        for ( size_t it = start; it != m_data.size() && m_data.at ( it ) . compareByTime ( to ) <= 0; ++ it ) {
                result.insert ( m_data . at ( it ) . from() );
                result.insert ( m_data . at ( it ) . to() );
        }
        return result;

    }
  private:
    bool findBound ( const CTimeStamp & elem , size_t & pos ) const {
        size_t low = 0, high = m_data.size();

        while (low < high) {
            size_t mid = low + (high - low) / 2;
            if (m_data[mid].compareByTime(elem) < 0) low = mid + 1;
            else high = mid;
        }

        pos = low;

        if (pos < m_data.size()) return true;
        return false;
    }

    int monthToNum ( const std::string & month ) {
        std::map < std::string, int > dictionary = { { "Jan", 1  },  { "Feb", 2  },  { "Mar", 3  },
                                                     { "Apr", 4  },  { "May", 5  },  { "Jun", 6  },
                                                     { "Jul", 7  },  { "Aug", 8  },  { "Sep", 9  },
                                                     { "Oct", 10 }, { "Nov", 11 }, { "Dec", 12 } };
        if ( dictionary.find ( month ) == dictionary.end() ) return 13;
        return dictionary.at ( month );
    }
    std::vector < CMail > m_data;
};

//----------------------------------------------------------------------------------------
#ifndef __PROGTEST__
} // namespace
std::string             printMail       ( const std::list<CMail> & all )
{
  std::ostringstream oss;
  for ( const auto & mail : all )
    oss << mail << "\n";
  return oss . str ();
}
int                     main ()
{
  MysteriousNamespace::CMailLog m;
  std::list<CMail> mailList;
  std::set<std::string> users;
  std::istringstream iss;

  iss . clear ();
  iss . str (
    "Mar 29 2025 12:35:32.233 relay.fit.cvut.cz ADFger72343D: from=user1@fit.cvut.cz\n"
    "Mar 29 2025 12:37:16.234 relay.fit.cvut.cz JlMSRW4232Df: from=person3@fit.cvut.cz\n"
    "Mar 29 2025 12:55:13.023 relay.fit.cvut.cz JlMSRW4232Df: subject=New progtest homework!\n"
    "Mar 29 2025 13:38:45.043 relay.fit.cvut.cz Kbced342sdgA: from=office13@fit.cvut.cz\n"
    "Mar 29 2025 13:36:13.023 relay.fit.cvut.cz JlMSRW4232Df: to=user76@fit.cvut.cz\n"
    "Mar 29 2025 13:55:31.456 relay.fit.cvut.cz KhdfEjkl247D: from=PR-department@fit.cvut.cz\n"
    "Mar 29 2025 14:18:12.654 relay.fit.cvut.cz Kbced342sdgA: to=boss13@fit.cvut.cz\n"
    "Mar 29 2025 14:48:32.563 relay.fit.cvut.cz KhdfEjkl247D: subject=Business partner\n"
    "Mar 29 2025 14:58:32.000 relay.fit.cvut.cz KhdfEjkl247D: to=HR-department@fit.cvut.cz\n"
    "Mar 29 2025 14:25:23.233 relay.fit.cvut.cz ADFger72343D: mail undeliverable\n"
    "Mar 29 2025 15:02:34.231 relay.fit.cvut.cz KhdfEjkl247D: to=CIO@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=CEO@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=dean@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=vice-dean@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=archive@fit.cvut.cz\n" );
  assert ( m . parseLog ( iss ) == 8 );
  mailList = m . listMail ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 23, 59, 59 ) );
  assert ( printMail ( mailList ) == R"###(2025-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!
2025-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz
2025-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> CEO@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> dean@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> vice-dean@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> archive@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.231 PR-department@fit.cvut.cz -> CIO@fit.cvut.cz, subject: Business partner
)###" );
  mailList = m . listMail ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 14, 58, 32 ) );
  assert ( printMail ( mailList ) == R"###(2025-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!
2025-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz
2025-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner
)###" );
  mailList = m . listMail ( CTimeStamp ( 2025, 3, 30, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 30, 23, 59, 59 ) );
  assert ( printMail ( mailList ) == "" );
  users = m . activeUsers ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 23, 59, 59 ) );
  assert ( users == std::set<std::string>( { "CEO@fit.cvut.cz", "CIO@fit.cvut.cz", "HR-department@fit.cvut.cz", "PR-department@fit.cvut.cz", "archive@fit.cvut.cz", "boss13@fit.cvut.cz", "dean@fit.cvut.cz", "office13@fit.cvut.cz", "person3@fit.cvut.cz", "user76@fit.cvut.cz", "vice-dean@fit.cvut.cz" } ) );
  users = m . activeUsers ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 13, 59, 59 ) );
  assert ( users == std::set<std::string>( { "person3@fit.cvut.cz", "user76@fit.cvut.cz" } ) );
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
