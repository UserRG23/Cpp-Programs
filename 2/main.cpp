#include <cstddef>
#include <ostream>
#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <span>
#include <algorithm>
#include <memory>
#include <compare>
#include <complex>
#endif /* __PROGTEST__ */

// Manipulator with additional functions and variables
std::string set_x = "x";

std::ostream& poly_var(std::ostream& os, const std::string& var) {
    set_x = var;
    return os;
}

struct SetPolyVar {
    std::string var;
    explicit SetPolyVar(const std::string& v) : var(v) {}
};

std::ostream& operator<<(std::ostream& os, const SetPolyVar& spv) {
    return poly_var(os, spv.var);
}

SetPolyVar poly_var(const std::string& var) {
    return SetPolyVar(var);
}

class CPolynomial
{
  public:
    // default constructor
      CPolynomial ( const double & num  = 0.0 ) : polynomial ( { num } ) {}

    // copy constructor
      CPolynomial ( const CPolynomial & o ) { polynomial = o.polynomial; }

    // operator * (polynomial, double)
      friend CPolynomial operator * ( const CPolynomial & first, const CPolynomial & second ) {
          CPolynomial result = first;
          return static_cast < CPolynomial > ( result *= second );
      }

    // operator *= (polynomial, double)
      CPolynomial & operator *= ( const CPolynomial & second ) {
          size_t firstdeg = degree(), seconddeg = second.degree();
          CPolynomial result;

          for ( size_t i = 0; i <= firstdeg; i ++ ) {
              for ( size_t j = 0; j <= seconddeg; j ++ ) {
                  result [ i + j ] += polynomial [ i ] * second [ j ];
              }
          }

          polynomial = result.polynomial;

          return *this;
      }

    // operator ==
      bool operator == ( const CPolynomial & o ) const {
          unsigned first = degree();
          unsigned second = o.degree();
          if ( first != second )
              return false;
          if ( ! std::equal ( polynomial.begin(), polynomial.begin() + first, o.polynomial.begin(), o.polynomial.begin() + second ) )
              return false;
          return true;
      }

    // operator !=
      bool operator != ( const CPolynomial & o ) const {
          return o.polynomial != polynomial;
      }

    // operator []
      double operator [] ( const size_t & indx ) const {
          return ( indx >= polynomial.size() ) ? 0.0 : polynomial [ indx ];
      }

      double & operator [] ( const size_t & indx ) {
          if ( indx >= polynomial.size() )
              polynomial.resize( indx + 1, 0 );
          return polynomial[ indx ];
      }

    // operator ()
      double operator () ( const double & x ) const {
          double result = 0;
          for ( size_t i = 0; i < polynomial.size(); i ++ ) {
              result += std::pow ( x, i ) * polynomial [ i ] ;
          }
          return result;
      }

    // operator !
      bool operator ! () const {
          if ( degree () == 0 ) return polynomial [ 0 ] == 0;
          else return false;
      }

    // cast to bool
      explicit operator bool () const {
          if ( degree () == 0 ) return polynomial [ 0 ] != 0;
          else return true;
      }

    // degree (), returns unsigned value
      unsigned degree () const {
          if (polynomial.empty() || (polynomial.size() == 1 && polynomial[0] == 0)) {
              return 0;
          }

          size_t result = polynomial.size() - 1;
          while ( polynomial [ result ] == 0 && result > 0 ) {
              result --;
          }
          return result;
      }

      friend std::ostream& operator << ( std::ostream& out, const CPolynomial & poly ) {
          size_t degree = poly.degree();
          bool firstTerm = true;

          for ( size_t i = degree; i > 0; --i ) {
              double coef = poly [ i ];
              if ( coef == 0 ) continue;

              if ( coef > 0 && !firstTerm ) out << " + ";
              if ( coef < 0 ) {
                  if ( firstTerm )
                      out << "- ";
                  else
                      out << " - ";
              }

              if ( std::abs ( coef ) != 1 ) {
                  out << std::abs (coef);
                  out << "*" << set_x << "^" << i;
              } else {
                  out << set_x << "^" << i;
              }

              firstTerm = false;
          }

          double constant = poly[ 0 ];
          if ( constant != 0 ) {
              if ( !firstTerm ) out << ( constant > 0 ? " + " : " - " );
              out << std::abs( constant );
          }

          if ( firstTerm ) out << "0";

          return out;
      }

  private:
      std::vector < double > polynomial;
};


#ifndef __PROGTEST__
bool smallDiff ( double a,
                 double b )
{
    return a == b;
}

bool dumpMatch ( const CPolynomial & x,
                 const std::vector<double> & ref )
{
    for ( size_t i = 0; i < ref.size(); i ++ ) {
        if ( ref [ i ] != x [ i ] )
            return false;
    }
    return true;
}

int main ()
{
  CPolynomial a, b, c;
  std::ostringstream out, tmp;

  a[0] = -10;
  a[1] = 3.5;
  a[3] = 1;
  assert ( smallDiff ( a ( 2 ), 5 ) );
  out . str ("");
  out << a;
  assert ( out . str () == "x^3 + 3.5*x^1 - 10" );
  c = a * -2;
  assert ( c . degree () == 3
           && dumpMatch ( c, std::vector<double>{ 20.0, -7.0, -0.0, -2.0 } ) );

  out . str ("");
  out << c;
  assert ( out . str () == "- 2*x^3 - 7*x^1 + 20" );
  out . str ("");
  out << b;
  assert ( out . str () == "0" );
  b[5] = -1;
  b[2] = 3;
  out . str ("");
  out << b;
  assert ( out . str () == "- x^5 + 3*x^2" );
  c = a * b;
  assert ( c . degree () == 8
           && dumpMatch ( c, std::vector<double>{ -0.0, -0.0, -30.0, 10.5, -0.0, 13.0, -3.5, 0.0, -1.0 } ) );

  out . str ("");
  out << c;
  assert ( out . str () == "- x^8 - 3.5*x^6 + 13*x^5 + 10.5*x^3 - 30*x^2" );
  a *= 5;
  assert ( a . degree () == 3
           && dumpMatch ( a, std::vector<double>{ -50.0, 17.5, 0.0, 5.0 } ) );

  a *= b;
  assert ( a . degree () == 8
           && dumpMatch ( a, std::vector<double>{ 0.0, 0.0, -150.0, 52.5, -0.0, 65.0, -17.5, -0.0, -5.0 } ) );

  assert ( a != b );
  b[5] = 0;
  assert ( static_cast<bool> ( b ) );
  assert ( ! ! b );
  b[2] = 0;
  assert ( !(a == b) );
  a *= 0;
  assert ( a . degree () == 0
           && dumpMatch ( a, std::vector<double>{ 0.0 } ) );

  assert ( a == b );
  assert ( ! static_cast<bool> ( b ) );
  assert ( ! b );

  // bonus - manipulators

  out . str ("");
  out << poly_var ( "y" ) << c;
  assert ( out . str () == "- y^8 - 3.5*y^6 + 13*y^5 + 10.5*y^3 - 30*y^2" );
  out . str ("");
  tmp << poly_var ( "abc" );
  out . copyfmt ( tmp );
  out << c;
  assert ( out . str () == "- abc^8 - 3.5*abc^6 + 13*abc^5 + 10.5*abc^3 - 30*abc^2" );
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
