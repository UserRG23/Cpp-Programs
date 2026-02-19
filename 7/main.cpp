#include <cstddef>
#ifndef __PROGTEST__
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <array>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <compare>
#include <algorithm>
#include <memory>
#include <iterator>
#include <functional>
#include <stdexcept>
#include <optional>
using namespace std::literals;

class CDumbString
{
  public:
    CDumbString             ( std::string v )
      : m_Data ( std::move ( v ) )
    {
    }
    bool        operator == ( const CDumbString & rhs ) const = default;
  private:
    std::string m_Data;
};
#endif /* __PROGTEST__ */

struct RMQ {
    RMQ () = default;
    explicit RMQ ( std::vector < size_t > data ) :
        data_ ( std::move  ( data ) ),
        block_ ( sqrt ( data_.size () ) + 1 ) {

        size_t min = 0;
        for ( size_t i = 0; i < data_.size(); i++ ) {
            if ( i > 0 && i % block_ == 0 ) {
                block_min_.push_back ( min );
                min = i;
            } else if ( data_[ i ] < data_ [ min ] ) min = i;
        }
        block_min_.push_back ( min );
    }

    const size_t find_min ( size_t from, size_t to ) const {
        const size_t* min = &data_ [ from ];
        size_t pos = from;
        while ( from < to ) {
            if ( from % block_ == 0 && from + block_ <= to ) {
                auto b_min = &data_ [ block_min_ [ from / block_ ] ];
                const size_t b_pos = block_min_ [ from / block_ ];
                if ( *b_min < *min ) {
                    min = b_min;
                    pos = b_pos;
                }
                from += block_;
            } else {
                auto x = &data_ [ from++ ];
                if ( *x < *min ) {
                    min = x;
                    pos = from - 1;
                }
            }
        }
        return pos;
    }

    private:
        std::vector < size_t > data_;
        size_t block_;
        std::vector < size_t > block_min_;
};

template <typename T_>
class CNet
{
  public:
    // default constructor
    CNet () {}

    // add ( x, y, cost )
    constexpr CNet & add ( const T_ & x, const T_ & y, const int & cost ) {
        auto itx = std::find ( m_all_names . begin (), m_all_names . end (), x );
        size_t posx = 0;
        if ( itx == m_all_names . end () ) {
            m_all_names . push_back ( x );
            posx = m_all_names . size () - 1;
        } else posx = itx - m_all_names . begin ();

        size_t posy = 0;
        auto ity = std::find ( m_all_names . begin (), m_all_names . end (), y );
        if ( ity == m_all_names . end () ) {
            m_all_names . push_back ( y );
            posy = m_all_names . size () - 1;
        } else posy = ity - m_all_names . begin ();

        if ( posx >= m_graph . size() )
            m_graph . push_back ( std::vector < std::pair < size_t, int > >
                                                ( { { posy, cost } } ) );
        else m_graph [ posx ] . push_back ( { posy, cost } );

        if ( posy >= m_graph . size() )
            m_graph . push_back ( std::vector < std::pair < size_t, int > >
                                                ( { { posx, cost } } ) );
        else m_graph [ posy ] . push_back ( { posx, cost } );

        return *this;
    }
    // optimize ()
    CNet & optimize () {
        std::vector < bool > visited ( m_all_names . size (), false );
        std::vector < size_t > depths;
        m_counted_cost . resize ( m_all_names . size () );

        dfs ( 0, 0, 0, visited, sequnce, depths );
        optimized = RMQ ( depths );
        return *this;
    }
    // totalCost ( x, y )

    const int totalCost ( const T_ & x, const T_ & y ) const {
        auto itx = std::find ( m_all_names . begin (), m_all_names . end (), x );
        auto ity = std::find ( m_all_names . begin (), m_all_names . end (), y );
        if ( itx == m_all_names . end () ||
             ity == m_all_names . end () )
            return -1;

        size_t numNamex = itx - m_all_names . begin ();
        size_t numNamey = ity - m_all_names . begin ();

        size_t posx = std::find ( sequnce . begin (), sequnce . end (), numNamex ) - sequnce . begin();
        size_t posy = std::find ( sequnce . begin (), sequnce . end (), numNamey ) - sequnce . begin();

        if ( posy == posx ) return 0;
        size_t lca = 0;
        if ( posx < posy ) lca = optimized . find_min ( posx, posy + 1 );
        if ( posy < posx ) lca = optimized . find_min ( posy, posx + 1 );

        size_t lcaNumName = sequnce . at ( lca );

        int total = m_counted_cost . at ( numNamex ) - m_counted_cost . at ( lcaNumName ) +
                    m_counted_cost . at ( numNamey ) - m_counted_cost . at ( lcaNumName );

        return total;
    }

    void print () const {
        for ( size_t i = 0; i < m_graph . size (); ++ i ) {
            std::cout << "Node " << i << ": ";
            for ( const auto & [ first, second ] : m_graph [ i ] ) {
            std::cout << first << ", ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
    }

  private:
      std::vector < std::vector < std::pair < size_t, int > > > m_graph;
      std::vector < size_t > sequnce;
      std::vector < size_t > m_counted_cost;
      std::vector < T_ > m_all_names;
      RMQ optimized;

      void dfs ( const size_t & root,
                 size_t depth,
                 size_t acc_cost,
                 std::vector < bool > & visited,
                 std::vector < size_t > & sequnce,
                 std::vector < size_t > & depths ) {
          visited [ root ] = true;
          m_counted_cost [ root ] = acc_cost;
          sequnce . push_back ( root );
          depths . push_back ( depth );

          for ( const auto & vertex : m_graph . at ( root ) ) {
              if ( visited . at ( vertex . first ) ) continue;
              dfs ( vertex . first, depth + 1, vertex . second + acc_cost, visited, sequnce, depths );
              sequnce . push_back ( root );
              depths . push_back ( depth );
          }
      }
};

#ifndef __PROGTEST__
int main ()
{
  CNet<std::string> a;
  a . add ( "Adam", "Bob", 100 )
    . add ( "Bob", "Carol", 200 )
    . add ( "Dave", "Adam", 300 )
    . add ( "Eve", "Fiona", 120 )
    . add ( "Kate", "Larry", 270 )
    . add ( "Ivan", "John", 70 )
    . add ( "Kate", "Ivan", 300 )
    . add ( "George", "Henry", 10 )
    . add ( "Eve", "George", 42 )
    . add ( "Adam", "Eve", 75 )
    . add ( "Ivan", "George", 38 )
    . optimize ();
  a . print ();
  assert ( a . totalCost ( "Adam", "Bob" ) == 100 );
  assert ( a . totalCost ( "John", "Eve" ) == 150 );
  assert ( a . totalCost ( "Dave", "Henry" ) == 427 );
  assert ( a . totalCost ( "Carol", "Larry" ) == 1025 );
  assert ( a . totalCost ( "George", "George" ) == 0 );
  assert ( a . totalCost ( "Alice", "Bob" ) == -1 );
  assert ( a . totalCost ( "Thomas", "Thomas" ) == -1 );

  CNet<int> b;
  b . add ( 0, 1, 100 )
    . add ( 1, 2, 200 )
    . add ( 3, 0, 300 )
    . add ( 4, 5, 120 )
    . add ( 10, 11, 270 )
    . add ( 8, 9, 70 )
    . add ( 10, 8, 300 )
    . add ( 6, 7, 10 )
    . add ( 4, 6, 42 )
    . add ( 0, 4, 75 )
    . add ( 8, 6, 38 )
    . optimize ();
  assert ( b . totalCost ( 0, 1 ) == 100 );
  assert ( b . totalCost ( 9, 4 ) == 150 );
  assert ( b . totalCost ( 3, 7 ) == 427 );
  assert ( b . totalCost ( 2, 11 ) == 1025 );
  assert ( b . totalCost ( 6, 6 ) == 0 );
  assert ( b . totalCost ( 0, 1 ) == 100 );
  assert ( b . totalCost ( 19, 19 ) == -1 );

  CNet<CDumbString> c;
  c . add ( "Adam"s, "Bob"s, 100 )
    . add ( "Bob"s, "Carol"s, 200 )
    . add ( "Dave"s, "Adam"s, 300 )
    . add ( "Eve"s, "Fiona"s, 120 )
    . add ( "Kate"s, "Larry"s, 270 )
    . add ( "Ivan"s, "John"s, 70 )
    . add ( "Kate"s, "Ivan"s, 300 )
    . add ( "George"s, "Henry"s, 10 )
    . add ( "Eve"s, "George"s, 42 )
    . add ( "Adam"s, "Eve"s, 75 )
    . add ( "Ivan"s, "George"s, 38 )
    . optimize ();
  assert ( c . totalCost ( "Adam"s, "Bob"s ) == 100 );
  assert ( c . totalCost ( "John"s, "Eve"s ) == 150 );
  assert ( c . totalCost ( "Dave"s, "Henry"s ) == 427 );
  assert ( c . totalCost ( "Carol"s, "Larry"s ) == 1025 );
  assert ( c . totalCost ( "George"s, "George"s ) == 0 );
  assert ( c . totalCost ( "Alice"s, "Bob"s ) == -1 );
  assert ( c . totalCost ( "Thomas"s, "Thomas"s ) == -1 );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
