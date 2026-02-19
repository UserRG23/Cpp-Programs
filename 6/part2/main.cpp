#ifndef __PROGTEST__
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cctype>
#include <cmath>
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
#include <functional>
#include <stdexcept>
#endif /* __PROGTEST__ */

class CCell {
    public:
        virtual ~CCell () = default;
        virtual size_t width () const = 0;
        virtual size_t hight () const = 0;
        virtual void render ( std::string & str, const size_t & width, const size_t & hight, const size_t & pos ) const = 0;
        virtual std::unique_ptr < CCell > clone () const = 0;
        virtual bool operator == ( const CCell & oth ) const = 0;
        virtual bool operator != ( const CCell & oth ) const { return !( *this == oth ); }
};

class CText : public CCell
{
    public:
        enum Align { ALIGN_RIGHT, ALIGN_LEFT };

        CText ( const std::string & str, const Align & al ) : m_align ( al ) {
            std::stringstream ss ( str );
            std::string tmp_str;
            while ( std::getline ( ss, tmp_str ) )
                m_lines . push_back ( tmp_str );
        }

        void setText ( const std::string & str ) {
            std::stringstream ss ( str );
            std::string tmp_str;
            m_lines = {};
            while ( std::getline ( ss, tmp_str ) )
                m_lines . push_back ( tmp_str );
        }

        void render ( std::string & str, const size_t & width, const size_t & hight, const size_t & pos ) const override {
            size_t hPad = 0; // horizontal padding
            if ( pos < m_lines . size () ) {
                if ( m_lines . at ( pos ) . size () < width ) hPad = ( width - m_lines . at ( pos ) . size () );
                if ( m_align == ALIGN_LEFT ) str += m_lines . at ( pos ) + std::string ( hPad, ' ' );
                else str += std::string ( hPad, ' ' ) + m_lines . at ( pos );
            } else {
                str += std::string ( width, ' ' );
            }
        }

        size_t width () const override {
            size_t width = 0;
            for ( const auto & line : m_lines )
                width = std::max ( line . size (), width );
            return width;
        }

        size_t hight () const override { return m_lines . size (); }

        bool operator == ( const CCell & oth ) const override {
            const CText * tmp = dynamic_cast < const CText * > ( & oth );
            return tmp != nullptr &&
                   tmp -> m_align == m_align &&
                   tmp -> m_lines == m_lines; }

        std::unique_ptr < CCell > clone () const override {
            return std::make_unique < CText > ( * this );
        }

    private:
        std::vector < std::string > m_lines;
        Align m_align;
};

class CEmpty : public CCell
{
    public:
        size_t width () const override { return 0; }
        size_t hight () const override { return 0; }

        std::unique_ptr < CCell > clone () const override {
            return std::make_unique < CEmpty > ( * this );
        }

        void render ( std::string & str, const size_t & width, const size_t & hight, const size_t & pos ) const override {
            str += std::string ( width, ' ' );
        }

        bool operator == ( const CCell & oth ) const override {
            return dynamic_cast < const CEmpty * > ( & oth ) != nullptr;
        }
};

class CImage : public CCell
{
    public:
        size_t width () const override { return m_lines . size () == 0 ? 0 : m_lines . at ( 0 ) . size (); }

        size_t hight () const override { return m_lines . size (); }

        void render ( std::string & str, const size_t & width, const size_t & hight, const size_t & pos ) const override {
            size_t upPad = 0; // vertical padding
            // size_t bottomPad = 0; // vertical padding
            size_t leftPad = 0; // horizontal padding
            size_t rightPad = 0; // horizontal padding
            if ( this -> hight() < hight ) {
                upPad = ( hight - this -> hight () ) / 2;
              //  bottomPad = upPad + ( hight - this -> hight () ) % 2;
            }
            if ( this -> width() < width ) {
                leftPad = ( width - this -> width () ) / 2;
                rightPad = leftPad + ( width - this -> width () ) % 2;
            }

            if ( upPad > pos && upPad != 0 ) {
                str += std::string ( width, ' ' );
                return;
            }
            if ( this -> hight () <= pos - upPad ) {
                str += std::string ( width, ' ' );
                return;
            }
            if ( this -> hight () > pos - upPad )
                str += std::string ( leftPad, ' ' )
                   + m_lines . at ( pos - upPad )
                   + std::string ( rightPad, ' ' );
        }

        CImage & addRow ( const std::string & str ) {
            std::stringstream ss ( str );
            std::string tmp;
            while ( std::getline ( ss, tmp ) )
                m_lines . push_back ( tmp );
            return *this;
        }

        std::unique_ptr < CCell > clone () const override {
            return std::make_unique < CImage > ( * this );
        }

        bool operator == ( const CCell & oth ) const override {
            const CImage * tmp = dynamic_cast < const CImage * > ( & oth );
            return tmp != nullptr && tmp -> m_lines == m_lines;
        }

    private:
        std::vector < std::string > m_lines;
};

class CTable
{
    public:
        ~CTable () = default;

        CTable ( const size_t & row, const size_t & col ) :
            m_col ( col ),
            m_row ( row )
            {
                m_table . resize ( col );
                for ( auto & cols : m_table ) {
                    cols . resize ( row );
                    for ( auto & cell : cols ) {
                        cell = std::make_unique < CEmpty > ();
                }
            }
        }

        CTable ( const CTable & oth ) :
            m_col ( oth . m_col ),
            m_row ( oth . m_row )
            {
                m_table . resize ( oth . m_col );
                for ( size_t col = 0; col < m_col; col ++ ) {
                    m_table [ col ] . resize ( oth . m_row );
                    for ( size_t row = 0; row < m_row; row ++ ) {
                        m_table [ col ] [ row ] = oth . m_table [ col ] [ row ] -> clone ();
                    }
                }
            }

        CTable & operator = ( const CTable & oth ) {
            m_col = oth . m_col;
            m_row = oth . m_row;

            m_table . resize ( m_col );
            for ( size_t col = 0; col < m_col; col ++ ) {
                m_table [ col ] . resize ( m_row );
                for ( size_t row = 0; row < m_row; row ++ ) {
                    m_table [ col ] [ row ] = oth . m_table [ col ] [ row ] -> clone ();
                }
            }
            return *this;
        }

        CCell & getCell ( const size_t & row, const size_t & col ) const {
            return * m_table [ col ] [ row ];
        }

        void setCell ( const size_t & row, const size_t & col, const CCell & newContent ) {
            m_table [ col ] [ row ] = newContent . clone ();
        }

        void setCell ( const size_t & row, const size_t & col, const CTable & table ) {
            std::stringstream os;
            os << table;
            m_table [ col ] [ row ] = std::make_unique < CText > ( CText ( os . str (), CText::ALIGN_LEFT ) );
        }

        bool operator == ( const CTable & oth ) const {
            if ( oth . m_col != m_col && oth . m_row != m_row ) return false;
            for ( size_t col = 0; col < m_col; col ++ ) {
                for ( size_t row = 0; row < m_row; row ++ ) {
                    if ( * oth . m_table . at ( col ) . at ( row ) != * m_table . at ( col ) . at ( row ) )
                        return false;
                }
            }
            return true;
        }

        bool operator != ( const CTable & oth ) const {
            return !( oth == *this );
        }

        friend std::ostream & operator << ( std::ostream & os, const CTable & oth ) {
            std::vector < size_t > hight ( oth . m_row, 0 );
            std::vector < size_t > width ( oth . m_col, 0 );
            for ( size_t col = 0; col < oth . m_col; col ++ ) {
                for ( size_t row = 0; row < oth . m_row; row ++ ) {
                    hight . at ( row ) = std::max ( oth . m_table . at ( col ) . at ( row ) -> hight (), hight . at ( row ) );
                    width . at ( col ) = std::max ( oth . m_table . at ( col ) . at ( row ) -> width (), width . at ( col ) );
                }
            }

            for ( size_t row = 0; row < oth . m_row; row ++ ) {
                for ( size_t col = 0; col < oth . m_col; col ++ )
                    os << '+' << std::string ( width . at ( col ), '-' );
                os << '+' << "\n";
                for ( size_t h = 0; h < hight . at ( row ); h ++ ) {
                    for ( size_t col = 0; col < oth . m_col; col ++ ) {
                            std::string line;
                            os << '|';
                            oth . m_table . at ( col ) . at ( row ) ->
                                render ( line, width . at ( col ), hight . at ( row ), h );

                            os << line;
                            if ( col + 1 == oth . m_col )
                                os << "|\n";
                    }
                }
            }

            for ( size_t col = 0; col < oth . m_col; col ++ )
                os << '+' << std::string ( width . at ( col ), '-' );
            os << '+' << '\n';

            return os;
        }

    private:
        size_t m_col = 0, m_row = 0;
        std::vector < std::vector < std::unique_ptr < CCell > > > m_table;
};


#ifndef __PROGTEST__
int main ()
{
  std::ostringstream oss;
  CTable t0 ( 3, 2 );
  t0 . setCell ( 0, 0, CText ( "Hello,\n"
        "Hello Kitty", CText::ALIGN_LEFT ) );
  t0 . setCell ( 1, 0, CText ( "Lorem ipsum dolor sit amet", CText::ALIGN_LEFT ) );
  t0 . setCell ( 2, 0, CText ( "Bye,\n"
        "Hello Kitty", CText::ALIGN_RIGHT ) );
  t0 . setCell ( 1, 1, CImage ()
          . addRow ( "###                   " )
          . addRow ( "#  #                  " )
          . addRow ( "#  # # ##   ###    ###" )
          . addRow ( "###  ##    #   #  #  #" )
          . addRow ( "#    #     #   #  #  #" )
          . addRow ( "#    #     #   #  #  #" )
          . addRow ( "#    #      ###    ###" )
          . addRow ( "                     #" )
          . addRow ( "                   ## " )
          . addRow ( "                      " )
          . addRow ( " #    ###   ###   #   " )
          . addRow ( "###  #   # #     ###  " )
          . addRow ( " #   #####  ###   #   " )
          . addRow ( " #   #         #  #   " )
          . addRow ( "  ##  ###   ###    ## " ) );
  t0 . setCell ( 2, 1, CEmpty () );
  oss . str ("");
  oss . clear ();
  oss << t0;
  assert ( oss . str () ==
        "+--------------------------+----------------------+\n"
        "|Hello,                    |                      |\n"
        "|Hello Kitty               |                      |\n"
        "+--------------------------+----------------------+\n"
        "|Lorem ipsum dolor sit amet|###                   |\n"
        "|                          |#  #                  |\n"
        "|                          |#  # # ##   ###    ###|\n"
        "|                          |###  ##    #   #  #  #|\n"
        "|                          |#    #     #   #  #  #|\n"
        "|                          |#    #     #   #  #  #|\n"
        "|                          |#    #      ###    ###|\n"
        "|                          |                     #|\n"
        "|                          |                   ## |\n"
        "|                          |                      |\n"
        "|                          | #    ###   ###   #   |\n"
        "|                          |###  #   # #     ###  |\n"
        "|                          | #   #####  ###   #   |\n"
        "|                          | #   #         #  #   |\n"
        "|                          |  ##  ###   ###    ## |\n"
        "+--------------------------+----------------------+\n"
        "|                      Bye,|                      |\n"
        "|               Hello Kitty|                      |\n"
        "+--------------------------+----------------------+\n" );
  t0 . setCell ( 0, 1, t0 . getCell ( 1, 1 ) );
  t0 . setCell ( 2, 1, CImage ()
          . addRow ( "*****   *      *  *      ******* ******  *" )
          . addRow ( "*    *  *      *  *      *            *  *" )
          . addRow ( "*    *  *      *  *      *           *   *" )
          . addRow ( "*    *  *      *  *      *****      *    *" )
          . addRow ( "****    *      *  *      *         *     *" )
          . addRow ( "*  *    *      *  *      *        *       " )
          . addRow ( "*   *   *      *  *      *       *       *" )
          . addRow ( "*    *    *****   ****** ******* ******  *" ) );
  dynamic_cast<CText &> ( t0 . getCell ( 1, 0 ) ) . setText ( "Lorem ipsum dolor sit amet,\n"
        "consectetur adipiscing\n"
        "elit. Curabitur scelerisque\n"
        "lorem vitae lectus cursus,\n"
        "vitae porta ante placerat. Class aptent taciti\n"
        "sociosqu ad litora\n"
        "torquent per\n"
        "conubia nostra,\n"
        "per inceptos himenaeos.\n"
        "\n"
        "Donec tincidunt augue\n"
        "sit amet metus\n"
        "pretium volutpat.\n"
        "Donec faucibus,\n"
        "ante sit amet\n"
        "luctus posuere,\n"
        "mauris tellus" );
  oss . str ("");
  oss . clear ();
  oss << t0;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  CTable t1 ( t0 );
  t1 . setCell ( 1, 0, CEmpty () );
  t1 . setCell ( 1, 1, CEmpty () );
  oss . str ("");
  oss . clear ();
  oss << t0;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  oss . str ("");
  oss . clear ();
  oss << t1;
  assert ( oss . str () ==
        "+-----------+------------------------------------------+\n"
        "|Hello,     |          ###                             |\n"
        "|Hello Kitty|          #  #                            |\n"
        "|           |          #  # # ##   ###    ###          |\n"
        "|           |          ###  ##    #   #  #  #          |\n"
        "|           |          #    #     #   #  #  #          |\n"
        "|           |          #    #     #   #  #  #          |\n"
        "|           |          #    #      ###    ###          |\n"
        "|           |                               #          |\n"
        "|           |                             ##           |\n"
        "|           |                                          |\n"
        "|           |           #    ###   ###   #             |\n"
        "|           |          ###  #   # #     ###            |\n"
        "|           |           #   #####  ###   #             |\n"
        "|           |           #   #         #  #             |\n"
        "|           |            ##  ###   ###    ##           |\n"
        "+-----------+------------------------------------------+\n"
        "+-----------+------------------------------------------+\n"
        "|       Bye,|*****   *      *  *      ******* ******  *|\n"
        "|Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|           |*    *  *      *  *      *           *   *|\n"
        "|           |*    *  *      *  *      *****      *    *|\n"
        "|           |****    *      *  *      *         *     *|\n"
        "|           |*  *    *      *  *      *        *       |\n"
        "|           |*   *   *      *  *      *       *       *|\n"
        "|           |*    *    *****   ****** ******* ******  *|\n"
        "+-----------+------------------------------------------+\n" );
  t1 = t0;
  t1 . setCell ( 0, 0, CEmpty () );
  t1 . setCell ( 1, 1, CImage ()
          . addRow ( "  ********                    " )
          . addRow ( " **********                   " )
          . addRow ( "**        **                  " )
          . addRow ( "**             **        **   " )
          . addRow ( "**             **        **   " )
          . addRow ( "***         ********  ********" )
          . addRow ( "****        ********  ********" )
          . addRow ( "****           **        **   " )
          . addRow ( "****           **        **   " )
          . addRow ( "****      **                  " )
          . addRow ( " **********                   " )
          . addRow ( "  ********                    " ) );
  oss . str ("");
  oss . clear ();
  oss << t0;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|Hello,                                        |          ###                             |\n"
        "|Hello Kitty                                   |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |          ###                             |\n"
        "|elit. Curabitur scelerisque                   |          #  #                            |\n"
        "|lorem vitae lectus cursus,                    |          #  # # ##   ###    ###          |\n"
        "|vitae porta ante placerat. Class aptent taciti|          ###  ##    #   #  #  #          |\n"
        "|sociosqu ad litora                            |          #    #     #   #  #  #          |\n"
        "|torquent per                                  |          #    #     #   #  #  #          |\n"
        "|conubia nostra,                               |          #    #      ###    ###          |\n"
        "|per inceptos himenaeos.                       |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|Donec tincidunt augue                         |                                          |\n"
        "|sit amet metus                                |           #    ###   ###   #             |\n"
        "|pretium volutpat.                             |          ###  #   # #     ###            |\n"
        "|Donec faucibus,                               |           #   #####  ###   #             |\n"
        "|ante sit amet                                 |           #   #         #  #             |\n"
        "|luctus posuere,                               |            ##  ###   ###    ##           |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  oss . str ("");
  oss . clear ();
  oss << t1;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                              |          ###                             |\n"
        "|                                              |          #  #                            |\n"
        "|                                              |          #  # # ##   ###    ###          |\n"
        "|                                              |          ###  ##    #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |                                          |\n"
        "|elit. Curabitur scelerisque                   |        ********                          |\n"
        "|lorem vitae lectus cursus,                    |       **********                         |\n"
        "|vitae porta ante placerat. Class aptent taciti|      **        **                        |\n"
        "|sociosqu ad litora                            |      **             **        **         |\n"
        "|torquent per                                  |      **             **        **         |\n"
        "|conubia nostra,                               |      ***         ********  ********      |\n"
        "|per inceptos himenaeos.                       |      ****        ********  ********      |\n"
        "|                                              |      ****           **        **         |\n"
        "|Donec tincidunt augue                         |      ****           **        **         |\n"
        "|sit amet metus                                |      ****      **                        |\n"
        "|pretium volutpat.                             |       **********                         |\n"
        "|Donec faucibus,                               |        ********                          |\n"
        "|ante sit amet                                 |                                          |\n"
        "|luctus posuere,                               |                                          |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  CTable t2 ( 2, 2 );
  t2 . setCell ( 0, 0, CText ( "OOP", CText::ALIGN_LEFT ) );
  t2 . setCell ( 0, 1, CText ( "Encapsulation", CText::ALIGN_LEFT ) );
  t2 . setCell ( 1, 0, CText ( "Polymorphism", CText::ALIGN_LEFT ) );
  t2 . setCell ( 1, 1, CText ( "Inheritance", CText::ALIGN_LEFT ) );
  oss . str ("");
  oss . clear ();
  oss << t2;
  assert ( oss . str () ==
        "+------------+-------------+\n"
        "|OOP         |Encapsulation|\n"
        "+------------+-------------+\n"
        "|Polymorphism|Inheritance  |\n"
        "+------------+-------------+\n" );
  t1 . setCell ( 0, 0, t2 );
  dynamic_cast<CText &> ( t2 . getCell ( 0, 0 ) ) . setText ( "Object Oriented Programming" );
  oss . str ("");
  oss . clear ();
  oss << t2;
  assert ( oss . str () ==
        "+---------------------------+-------------+\n"
        "|Object Oriented Programming|Encapsulation|\n"
        "+---------------------------+-------------+\n"
        "|Polymorphism               |Inheritance  |\n"
        "+---------------------------+-------------+\n" );
  oss . str ("");
  oss . clear ();
  oss << t1;
  assert ( oss . str () ==
        "+----------------------------------------------+------------------------------------------+\n"
        "|+------------+-------------+                  |          ###                             |\n"
        "||OOP         |Encapsulation|                  |          #  #                            |\n"
        "|+------------+-------------+                  |          #  # # ##   ###    ###          |\n"
        "||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          |\n"
        "|+------------+-------------+                  |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #     #   #  #  #          |\n"
        "|                                              |          #    #      ###    ###          |\n"
        "|                                              |                               #          |\n"
        "|                                              |                             ##           |\n"
        "|                                              |                                          |\n"
        "|                                              |           #    ###   ###   #             |\n"
        "|                                              |          ###  #   # #     ###            |\n"
        "|                                              |           #   #####  ###   #             |\n"
        "|                                              |           #   #         #  #             |\n"
        "|                                              |            ##  ###   ###    ##           |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                   |                                          |\n"
        "|consectetur adipiscing                        |                                          |\n"
        "|elit. Curabitur scelerisque                   |        ********                          |\n"
        "|lorem vitae lectus cursus,                    |       **********                         |\n"
        "|vitae porta ante placerat. Class aptent taciti|      **        **                        |\n"
        "|sociosqu ad litora                            |      **             **        **         |\n"
        "|torquent per                                  |      **             **        **         |\n"
        "|conubia nostra,                               |      ***         ********  ********      |\n"
        "|per inceptos himenaeos.                       |      ****        ********  ********      |\n"
        "|                                              |      ****           **        **         |\n"
        "|Donec tincidunt augue                         |      ****           **        **         |\n"
        "|sit amet metus                                |      ****      **                        |\n"
        "|pretium volutpat.                             |       **********                         |\n"
        "|Donec faucibus,                               |        ********                          |\n"
        "|ante sit amet                                 |                                          |\n"
        "|luctus posuere,                               |                                          |\n"
        "|mauris tellus                                 |                                          |\n"
        "+----------------------------------------------+------------------------------------------+\n"
        "|                                          Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                   Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                              |*    *  *      *  *      *           *   *|\n"
        "|                                              |*    *  *      *  *      *****      *    *|\n"
        "|                                              |****    *      *  *      *         *     *|\n"
        "|                                              |*  *    *      *  *      *        *       |\n"
        "|                                              |*   *   *      *  *      *       *       *|\n"
        "|                                              |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------+------------------------------------------+\n" );
  assert ( t0 != t1 );
  assert ( !( t0 == t1 ) );
  assert ( t0 . getCell ( 1, 1 ) == t0 . getCell ( 0, 1 ) );
  assert ( ! ( t0 . getCell ( 1, 1 ) != t0 . getCell ( 0, 1 ) ) );
  assert ( t0 . getCell ( 0, 0 ) != t0 . getCell ( 0, 1 ) );
  assert ( ! ( t0 . getCell ( 0, 0 ) == t0 . getCell ( 0, 1 ) ) );
  t1 . setCell ( 0, 0, t1 );
  oss . str ("");
  oss . clear ();
  oss << t1;
  assert ( oss . str () ==
        "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|+----------------------------------------------+------------------------------------------+|                                          |\n"
        "||+------------+-------------+                  |          ###                             ||                                          |\n"
        "|||OOP         |Encapsulation|                  |          #  #                            ||                                          |\n"
        "||+------------+-------------+                  |          #  # # ##   ###    ###          ||                                          |\n"
        "|||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          ||                                          |\n"
        "||+------------+-------------+                  |          #    #     #   #  #  #          ||                                          |\n"
        "||                                              |          #    #     #   #  #  #          ||                                          |\n"
        "||                                              |          #    #      ###    ###          ||                                          |\n"
        "||                                              |                               #          ||                                          |\n"
        "||                                              |                             ##           ||                                          |\n"
        "||                                              |                                          ||                                          |\n"
        "||                                              |           #    ###   ###   #             ||                                          |\n"
        "||                                              |          ###  #   # #     ###            ||                                          |\n"
        "||                                              |           #   #####  ###   #             ||                                          |\n"
        "||                                              |           #   #         #  #             ||          ###                             |\n"
        "||                                              |            ##  ###   ###    ##           ||          #  #                            |\n"
        "|+----------------------------------------------+------------------------------------------+|          #  # # ##   ###    ###          |\n"
        "||Lorem ipsum dolor sit amet,                   |                                          ||          ###  ##    #   #  #  #          |\n"
        "||consectetur adipiscing                        |                                          ||          #    #     #   #  #  #          |\n"
        "||elit. Curabitur scelerisque                   |        ********                          ||          #    #     #   #  #  #          |\n"
        "||lorem vitae lectus cursus,                    |       **********                         ||          #    #      ###    ###          |\n"
        "||vitae porta ante placerat. Class aptent taciti|      **        **                        ||                               #          |\n"
        "||sociosqu ad litora                            |      **             **        **         ||                             ##           |\n"
        "||torquent per                                  |      **             **        **         ||                                          |\n"
        "||conubia nostra,                               |      ***         ********  ********      ||           #    ###   ###   #             |\n"
        "||per inceptos himenaeos.                       |      ****        ********  ********      ||          ###  #   # #     ###            |\n"
        "||                                              |      ****           **        **         ||           #   #####  ###   #             |\n"
        "||Donec tincidunt augue                         |      ****           **        **         ||           #   #         #  #             |\n"
        "||sit amet metus                                |      ****      **                        ||            ##  ###   ###    ##           |\n"
        "||pretium volutpat.                             |       **********                         ||                                          |\n"
        "||Donec faucibus,                               |        ********                          ||                                          |\n"
        "||ante sit amet                                 |                                          ||                                          |\n"
        "||luctus posuere,                               |                                          ||                                          |\n"
        "||mauris tellus                                 |                                          ||                                          |\n"
        "|+----------------------------------------------+------------------------------------------+|                                          |\n"
        "||                                          Bye,|*****   *      *  *      ******* ******  *||                                          |\n"
        "||                                   Hello Kitty|*    *  *      *  *      *            *  *||                                          |\n"
        "||                                              |*    *  *      *  *      *           *   *||                                          |\n"
        "||                                              |*    *  *      *  *      *****      *    *||                                          |\n"
        "||                                              |****    *      *  *      *         *     *||                                          |\n"
        "||                                              |*  *    *      *  *      *        *       ||                                          |\n"
        "||                                              |*   *   *      *  *      *       *       *||                                          |\n"
        "||                                              |*    *    *****   ****** ******* ******  *||                                          |\n"
        "|+----------------------------------------------+------------------------------------------+|                                          |\n"
        "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                                                                |                                          |\n"
        "|consectetur adipiscing                                                                     |                                          |\n"
        "|elit. Curabitur scelerisque                                                                |        ********                          |\n"
        "|lorem vitae lectus cursus,                                                                 |       **********                         |\n"
        "|vitae porta ante placerat. Class aptent taciti                                             |      **        **                        |\n"
        "|sociosqu ad litora                                                                         |      **             **        **         |\n"
        "|torquent per                                                                               |      **             **        **         |\n"
        "|conubia nostra,                                                                            |      ***         ********  ********      |\n"
        "|per inceptos himenaeos.                                                                    |      ****        ********  ********      |\n"
        "|                                                                                           |      ****           **        **         |\n"
        "|Donec tincidunt augue                                                                      |      ****           **        **         |\n"
        "|sit amet metus                                                                             |      ****      **                        |\n"
        "|pretium volutpat.                                                                          |       **********                         |\n"
        "|Donec faucibus,                                                                            |        ********                          |\n"
        "|ante sit amet                                                                              |                                          |\n"
        "|luctus posuere,                                                                            |                                          |\n"
        "|mauris tellus                                                                              |                                          |\n"
        "+-------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|                                                                                       Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                                                                Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                                                                           |*    *  *      *  *      *           *   *|\n"
        "|                                                                                           |*    *  *      *  *      *****      *    *|\n"
        "|                                                                                           |****    *      *  *      *         *     *|\n"
        "|                                                                                           |*  *    *      *  *      *        *       |\n"
        "|                                                                                           |*   *   *      *  *      *       *       *|\n"
        "|                                                                                           |*    *    *****   ****** ******* ******  *|\n"
        "+-------------------------------------------------------------------------------------------+------------------------------------------+\n" );
  t1 . setCell ( 0, 0, t1 );
  oss . str ("");
  oss . clear ();
  oss << t1;
  assert ( oss . str () ==
        "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
        "||+----------------------------------------------+------------------------------------------+|                                          ||                                          |\n"
        "|||+------------+-------------+                  |          ###                             ||                                          ||                                          |\n"
        "||||OOP         |Encapsulation|                  |          #  #                            ||                                          ||                                          |\n"
        "|||+------------+-------------+                  |          #  # # ##   ###    ###          ||                                          ||                                          |\n"
        "||||Polymorphism|Inheritance  |                  |          ###  ##    #   #  #  #          ||                                          ||                                          |\n"
        "|||+------------+-------------+                  |          #    #     #   #  #  #          ||                                          ||                                          |\n"
        "|||                                              |          #    #     #   #  #  #          ||                                          ||                                          |\n"
        "|||                                              |          #    #      ###    ###          ||                                          ||                                          |\n"
        "|||                                              |                               #          ||                                          ||                                          |\n"
        "|||                                              |                             ##           ||                                          ||                                          |\n"
        "|||                                              |                                          ||                                          ||                                          |\n"
        "|||                                              |           #    ###   ###   #             ||                                          ||                                          |\n"
        "|||                                              |          ###  #   # #     ###            ||                                          ||                                          |\n"
        "|||                                              |           #   #####  ###   #             ||                                          ||                                          |\n"
        "|||                                              |           #   #         #  #             ||          ###                             ||                                          |\n"
        "|||                                              |            ##  ###   ###    ##           ||          #  #                            ||                                          |\n"
        "||+----------------------------------------------+------------------------------------------+|          #  # # ##   ###    ###          ||                                          |\n"
        "|||Lorem ipsum dolor sit amet,                   |                                          ||          ###  ##    #   #  #  #          ||                                          |\n"
        "|||consectetur adipiscing                        |                                          ||          #    #     #   #  #  #          ||                                          |\n"
        "|||elit. Curabitur scelerisque                   |        ********                          ||          #    #     #   #  #  #          ||                                          |\n"
        "|||lorem vitae lectus cursus,                    |       **********                         ||          #    #      ###    ###          ||                                          |\n"
        "|||vitae porta ante placerat. Class aptent taciti|      **        **                        ||                               #          ||                                          |\n"
        "|||sociosqu ad litora                            |      **             **        **         ||                             ##           ||                                          |\n"
        "|||torquent per                                  |      **             **        **         ||                                          ||                                          |\n"
        "|||conubia nostra,                               |      ***         ********  ********      ||           #    ###   ###   #             ||                                          |\n"
        "|||per inceptos himenaeos.                       |      ****        ********  ********      ||          ###  #   # #     ###            ||                                          |\n"
        "|||                                              |      ****           **        **         ||           #   #####  ###   #             ||                                          |\n"
        "|||Donec tincidunt augue                         |      ****           **        **         ||           #   #         #  #             ||                                          |\n"
        "|||sit amet metus                                |      ****      **                        ||            ##  ###   ###    ##           ||          ###                             |\n"
        "|||pretium volutpat.                             |       **********                         ||                                          ||          #  #                            |\n"
        "|||Donec faucibus,                               |        ********                          ||                                          ||          #  # # ##   ###    ###          |\n"
        "|||ante sit amet                                 |                                          ||                                          ||          ###  ##    #   #  #  #          |\n"
        "|||luctus posuere,                               |                                          ||                                          ||          #    #     #   #  #  #          |\n"
        "|||mauris tellus                                 |                                          ||                                          ||          #    #     #   #  #  #          |\n"
        "||+----------------------------------------------+------------------------------------------+|                                          ||          #    #      ###    ###          |\n"
        "|||                                          Bye,|*****   *      *  *      ******* ******  *||                                          ||                               #          |\n"
        "|||                                   Hello Kitty|*    *  *      *  *      *            *  *||                                          ||                             ##           |\n"
        "|||                                              |*    *  *      *  *      *           *   *||                                          ||                                          |\n"
        "|||                                              |*    *  *      *  *      *****      *    *||                                          ||           #    ###   ###   #             |\n"
        "|||                                              |****    *      *  *      *         *     *||                                          ||          ###  #   # #     ###            |\n"
        "|||                                              |*  *    *      *  *      *        *       ||                                          ||           #   #####  ###   #             |\n"
        "|||                                              |*   *   *      *  *      *       *       *||                                          ||           #   #         #  #             |\n"
        "|||                                              |*    *    *****   ****** ******* ******  *||                                          ||            ##  ###   ###    ##           |\n"
        "||+----------------------------------------------+------------------------------------------+|                                          ||                                          |\n"
        "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
        "||Lorem ipsum dolor sit amet,                                                                |                                          ||                                          |\n"
        "||consectetur adipiscing                                                                     |                                          ||                                          |\n"
        "||elit. Curabitur scelerisque                                                                |        ********                          ||                                          |\n"
        "||lorem vitae lectus cursus,                                                                 |       **********                         ||                                          |\n"
        "||vitae porta ante placerat. Class aptent taciti                                             |      **        **                        ||                                          |\n"
        "||sociosqu ad litora                                                                         |      **             **        **         ||                                          |\n"
        "||torquent per                                                                               |      **             **        **         ||                                          |\n"
        "||conubia nostra,                                                                            |      ***         ********  ********      ||                                          |\n"
        "||per inceptos himenaeos.                                                                    |      ****        ********  ********      ||                                          |\n"
        "||                                                                                           |      ****           **        **         ||                                          |\n"
        "||Donec tincidunt augue                                                                      |      ****           **        **         ||                                          |\n"
        "||sit amet metus                                                                             |      ****      **                        ||                                          |\n"
        "||pretium volutpat.                                                                          |       **********                         ||                                          |\n"
        "||Donec faucibus,                                                                            |        ********                          ||                                          |\n"
        "||ante sit amet                                                                              |                                          ||                                          |\n"
        "||luctus posuere,                                                                            |                                          ||                                          |\n"
        "||mauris tellus                                                                              |                                          ||                                          |\n"
        "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
        "||                                                                                       Bye,|*****   *      *  *      ******* ******  *||                                          |\n"
        "||                                                                                Hello Kitty|*    *  *      *  *      *            *  *||                                          |\n"
        "||                                                                                           |*    *  *      *  *      *           *   *||                                          |\n"
        "||                                                                                           |*    *  *      *  *      *****      *    *||                                          |\n"
        "||                                                                                           |****    *      *  *      *         *     *||                                          |\n"
        "||                                                                                           |*  *    *      *  *      *        *       ||                                          |\n"
        "||                                                                                           |*   *   *      *  *      *       *       *||                                          |\n"
        "||                                                                                           |*    *    *****   ****** ******* ******  *||                                          |\n"
        "|+-------------------------------------------------------------------------------------------+------------------------------------------+|                                          |\n"
        "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|Lorem ipsum dolor sit amet,                                                                                                             |                                          |\n"
        "|consectetur adipiscing                                                                                                                  |                                          |\n"
        "|elit. Curabitur scelerisque                                                                                                             |        ********                          |\n"
        "|lorem vitae lectus cursus,                                                                                                              |       **********                         |\n"
        "|vitae porta ante placerat. Class aptent taciti                                                                                          |      **        **                        |\n"
        "|sociosqu ad litora                                                                                                                      |      **             **        **         |\n"
        "|torquent per                                                                                                                            |      **             **        **         |\n"
        "|conubia nostra,                                                                                                                         |      ***         ********  ********      |\n"
        "|per inceptos himenaeos.                                                                                                                 |      ****        ********  ********      |\n"
        "|                                                                                                                                        |      ****           **        **         |\n"
        "|Donec tincidunt augue                                                                                                                   |      ****           **        **         |\n"
        "|sit amet metus                                                                                                                          |      ****      **                        |\n"
        "|pretium volutpat.                                                                                                                       |       **********                         |\n"
        "|Donec faucibus,                                                                                                                         |        ********                          |\n"
        "|ante sit amet                                                                                                                           |                                          |\n"
        "|luctus posuere,                                                                                                                         |                                          |\n"
        "|mauris tellus                                                                                                                           |                                          |\n"
        "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n"
        "|                                                                                                                                    Bye,|*****   *      *  *      ******* ******  *|\n"
        "|                                                                                                                             Hello Kitty|*    *  *      *  *      *            *  *|\n"
        "|                                                                                                                                        |*    *  *      *  *      *           *   *|\n"
        "|                                                                                                                                        |*    *  *      *  *      *****      *    *|\n"
        "|                                                                                                                                        |****    *      *  *      *         *     *|\n"
        "|                                                                                                                                        |*  *    *      *  *      *        *       |\n"
        "|                                                                                                                                        |*   *   *      *  *      *       *       *|\n"
        "|                                                                                                                                        |*    *    *****   ****** ******* ******  *|\n"
        "+----------------------------------------------------------------------------------------------------------------------------------------+------------------------------------------+\n" );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
