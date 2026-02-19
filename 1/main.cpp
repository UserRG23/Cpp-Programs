#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <compare>
#include <functional>
#include <stdexcept>
#include <algorithm>
#endif /* __PROGTEST__ */

struct Registration { // data of the account 
    std::string name;
    std::string addr;
    std::string account;
    unsigned sumIncome = 0; 
    unsigned sumExpense = 0;

    bool operator== ( Registration const & oth ) {
        return oth.name == name && oth.addr == addr;
    }

    Registration ( const std::string & othrName, const std::string & othrAddr ) : name ( othrName ), addr ( othrAddr ) {}
    Registration ( const std::string & othrName, const std::string & othrAddr, const std::string & othrAcc ) : name ( othrName ), addr ( othrAddr ), account ( othrAcc ) {}
};

struct Acc {
    std::string name;
    std::string addr;
    std::string account;

    bool operator== ( std::string const & othAcc ) {
        return othAcc == account;
    }

    Acc ( const std::string & othrName, const std::string & othrAddr, const std::string & othrAcc ) : name ( othrName ), addr ( othrAddr ), account ( othrAcc ) {}
};

bool FindN ( const Registration & first, const Registration & second ) {
    if ( first.name != second.name ) return first.name < second.name;
    return first.addr < second.addr;
}

bool FindA ( const Acc & first, std::string second ) {
    return first.account < second;
}

class CIterator
{
  public:
      bool                atEnd   () const {
          return it == data.size();
      }

      void                next    () {
          it ++;
      }

      const std::string & name    () const {
          return data[ it ].name;
      }

      const std::string & addr    () const {
          return data[ it ].addr;
      }

      const std::string & account () const {
          return data[ it ].account;
      }

      CIterator ( std::vector < Registration > const & oth, size_t const & othIt ) : data ( oth ), it ( othIt ) {}

  private:
    std::vector < Registration > data;
    size_t it;
};

class CTaxRegister
{
  public:
    bool      birth      ( const std::string    & name,
                           const std::string    & addr,
                           const std::string    & account )
    {
        auto it = std::lower_bound ( m_data.begin(), m_data.end(), Registration ( name, addr ), FindN );
        auto it2 = std::lower_bound ( m_account.begin(), m_account.end(), account, FindA );

        if ( it != m_data.end() && it->name == name && it->addr == addr ) return false;
        if ( it2 != m_account.end() && it2->account == account ) return false;

        m_data.insert ( it, Registration ( name, addr, account ) );
        m_account.insert ( it2, Acc ( name, addr, account ) );
        return true;
    }

    bool      death      ( const std::string    & name,
                           const std::string    & addr )
    {
        auto it = std::lower_bound ( m_data.begin(), m_data.end(), Registration ( name, addr ), FindN );
        if ( it == m_data.end() || it->addr != addr || it->name != name ) return false;
        auto it2 = std::lower_bound ( m_account.begin(), m_account.end(), it->account, FindA );
        m_data.erase( it );
        m_account.erase( it2 );
        return true;
    }

    bool      income     ( const std::string    & account,
                           int                    amount )
    {
        auto it = std::lower_bound ( m_account.begin(), m_account.end(), account, FindA );
        if ( it == m_account.end() || it->account != account ) return false;
        auto it2 = std::lower_bound ( m_data.begin(), m_data.end(), Registration ( it->name, it->addr ), FindN );
        it2->sumIncome += amount;
        return true;
    }

    bool      income     ( const std::string    & name,
                           const std::string    & addr,
                           int                    amount ) 
    {
        auto it = std::lower_bound ( m_data.begin(), m_data.end(), Registration ( name, addr ), FindN );
        if ( it == m_data.end() || it->name != name || it->addr != addr ) return false;
        it->sumIncome += amount;
        return true;
    }

    bool      expense    ( const std::string    & account,
                           int                    amount ) 
    {
        auto it = std::lower_bound ( m_account.begin(), m_account.end(), account, FindA );
        if ( it == m_account.end() || it->account != account ) return false;
        auto it2 = std::lower_bound ( m_data.begin(), m_data.end(), Registration ( it->name, it->addr ), FindN );
        it2->sumExpense += amount;
        return true;
    }

    bool      expense    ( const std::string    & name,
                           const std::string    & addr,
                           int                    amount )
    {
        auto it = std::lower_bound ( m_data.begin(), m_data.end(), Registration ( name, addr ), FindN );
        if ( it == m_data.end() || it->name != name || it->addr != addr ) return false;
        it->sumExpense += amount;
        return true;
    }

    bool      audit      ( const std::string    & name,
                           const std::string    & addr,
                           std::string          & account,
                           int                  & sumIncome,
                           int                  & sumExpense ) const 
    {
        auto it = std::lower_bound ( m_data.begin(), m_data.end(), Registration ( name, addr ), FindN );
        if ( it == m_data.end() || it->name != name || it->addr != addr ) return false;

        account = it->account;
        sumIncome = it->sumIncome;
        sumExpense = it->sumExpense;
        return true;
    }
   
    CIterator listByName () const 
    {
        return CIterator ( m_data, 0 );
    }

  private:

    std::vector < Acc > m_account;
    std::vector < Registration > m_data;
};


#ifndef __PROGTEST__
int main ()
{
  std::string acct;
  int    sumIncome, sumExpense;
  CTaxRegister b0;
  assert ( b0 . birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
  assert ( b0 . birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
  assert ( b0 . birth ( "Peter Hacker", "Main Street 17", "634oddT" ) );
  assert ( b0 . birth ( "John Smith", "Main Street 17", "Z343rwZ" ) );
  assert ( b0 . income ( "Xuj5#94", 1000 ) );
  assert ( b0 . income ( "634oddT", 2000 ) );
  assert ( b0 . income ( "123/456/789", 3000 ) );
  assert ( b0 . income ( "634oddT", 4000 ) );
  assert ( b0 . income ( "Peter Hacker", "Main Street 17", 2000 ) );
  assert ( b0 . expense ( "Jane Hacker", "Main Street 17", 2000 ) );
  assert ( b0 . expense ( "John Smith", "Main Street 17", 500 ) );
  assert ( b0 . expense ( "Jane Hacker", "Main Street 17", 1000 ) );
  assert ( b0 . expense ( "Xuj5#94", 1300 ) );
  assert ( b0 . audit ( "John Smith", "Oak Road 23", acct, sumIncome, sumExpense ) );
  assert ( acct == "123/456/789" );
  assert ( sumIncome == 3000 );
  assert ( sumExpense == 0 );
  assert ( b0 . audit ( "Jane Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "Xuj5#94" );
  assert ( sumIncome == 1000 );
  assert ( sumExpense == 4300 );
  assert ( b0 . audit ( "Peter Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "634oddT" );
  assert ( sumIncome == 8000 );
  assert ( sumExpense == 0 );
  assert ( b0 . audit ( "John Smith", "Main Street 17", acct, sumIncome, sumExpense ) );
  assert ( acct == "Z343rwZ" );
  assert ( sumIncome == 0 );
  assert ( sumExpense == 500 );
  CIterator it = b0 . listByName ();
  assert ( ! it . atEnd ()
           && it . name () == "Jane Hacker"
           && it . addr () == "Main Street 17"
           && it . account () == "Xuj5#94" );
  it . next ();
  assert ( ! it . atEnd ()
           && it . name () == "John Smith"
           && it . addr () == "Main Street 17"
           && it . account () == "Z343rwZ" );
  it . next ();
  assert ( ! it . atEnd ()
           && it . name () == "John Smith"
           && it . addr () == "Oak Road 23"
           && it . account () == "123/456/789" );
  it . next ();
  assert ( ! it . atEnd ()
           && it . name () == "Peter Hacker"
           && it . addr () == "Main Street 17"
           && it . account () == "634oddT" );
  it . next ();
  assert ( it . atEnd () );

  assert ( b0 . death ( "John Smith", "Main Street 17" ) );

  CTaxRegister b1;
  assert ( b1 . birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
  assert ( b1 . birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
  assert ( !b1 . income ( "634oddT", 4000 ) );
  assert ( !b1 . expense ( "John Smith", "Main Street 18", 500 ) );
  assert ( !b1 . audit ( "John Nowak", "Second Street 23", acct, sumIncome, sumExpense ) );
  assert ( !b1 . death ( "Peter Nowak", "5-th Avenue" ) );
  assert ( !b1 . birth ( "Jane Hacker", "Main Street 17", "4et689A" ) );
  assert ( !b1 . birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
  assert ( b1 . death ( "Jane Hacker", "Main Street 17" ) );
  assert ( b1 . birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
  assert ( b1 . audit ( "Joe Hacker", "Elm Street 23", acct, sumIncome, sumExpense ) );
  assert ( acct == "Xuj5#94" );
  assert ( sumIncome == 0 );
  assert ( sumExpense == 0 );
  assert ( !b1 . birth ( "Joe Hacker", "Elm Street 23", "AAj5#94" ) );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
