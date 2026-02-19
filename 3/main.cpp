#include <cstddef>
#include <ostream>
#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#endif /* __PROGTEST__ */

struct Domicile {
    char *date;
    char *street;
    char *city;
    Domicile *next;
    
    Domicile(const char *d, const char *s, const char *c) : next(nullptr) {
        date = strdup(d);
        street = strdup(s);
        city = strdup(c);
    }
    
    ~Domicile() {
        free(date);
        free(street);
        free(city);
    }

    // Deep copy function
    Domicile *clone() const {
        Domicile *copy = new Domicile(date, street, city);
        if (next) copy->next = next->clone();
        return copy;
    }
};

struct Citizen {
    char *id;
    char *name;
    char *surname;
    Domicile *domiciles;
    Citizen *next;
    
    Citizen(const char *i, const char *n, const char *s, const char *d, const char *st, const char *c) : next(nullptr) {
        id = strdup(i);
        name = strdup(n);
        surname = strdup(s);
        domiciles = new Domicile(d, st, c);
    }
    
    ~Citizen() {
        free(id);
        free(name);
        free(surname);
        while (domiciles) {
            Domicile *tmp = domiciles;
            domiciles = domiciles->next;
            delete tmp;
        }
    }

    // Deep copy function
    Citizen *clone() const {
        Citizen *copy = new Citizen(id, name, surname, domiciles->date, domiciles->street, domiciles->city);
        Domicile *srcDomicile = domiciles->next, *lastDomicile = copy->domiciles;
        while (srcDomicile) {
            lastDomicile->next = new Domicile(srcDomicile->date, srcDomicile->street, srcDomicile->city);
            lastDomicile = lastDomicile->next;
            srcDomicile = srcDomicile->next;
        }
        if (next) copy->next = next->clone();
        return copy;
    }
};

class CRegister {
private:
    Citizen *head;
    int *refCount;  // Reference counter for copy-on-write

    void copyFrom(const CRegister &src) {
        head = src.head;
        refCount = src.refCount;
        if (refCount) (*refCount)++;
    }

    void ensureUnique() {
        if (*refCount == 1) return;  // Already unique, no need to copy

        // Deep copy required
        Citizen *newHead = nullptr, *last = nullptr;
        for (Citizen *c = head; c; c = c->next) {
            Citizen *newCitizen = c->clone();
            if (!newHead) newHead = newCitizen;
            else last->next = newCitizen;
            last = newCitizen;
        }
        
        // Reduce refCount for old data and create new counter
        if (--(*refCount) == 0) {
            delete head;
            delete refCount;
        }

        head = newHead;
        refCount = new int(1);
    }

public:
    CRegister() : head(nullptr), refCount(new int(1)) {}

    CRegister(const CRegister &src) : head(nullptr), refCount(nullptr) {
        copyFrom(src);
    }

    ~CRegister() {
        if (--(*refCount) == 0) {
            while (head) {
                Citizen *tmp = head;
                head = head->next;
                delete tmp;
            }
            delete refCount;
        }
    }

    CRegister &operator=(const CRegister &src) {
        if (this != &src) {
            if (--(*refCount) == 0) {
                while (head) {
                    Citizen *tmp = head;
                    head = head->next;
                    delete tmp;
                }
                delete refCount;
            }
            copyFrom(src);
        }
        return *this;
    }

    bool add(const char *id, const char *name, const char *surname, const char *date, const char *street, const char *city) {
        ensureUnique();  // Ensure unique ownership before modifying

        for (Citizen *c = head; c; c = c->next) {
            if (strcmp(c->id, id) == 0) return false;
        }
        Citizen *newCitizen = new Citizen(id, name, surname, date, street, city);
        newCitizen->next = head;
        head = newCitizen;
        return true;
    }

    bool resettle(const char *id, const char *date, const char *street, const char *city) {
        ensureUnique();  // Ensure unique ownership before modifying

        for (Citizen *c = head; c; c = c->next) {
            if (strcmp(c->id, id) == 0) {
                for (Domicile *d = c->domiciles; d; d = d->next) {
                    if (strcmp(d->date, date) == 0) return false;
                }
                Domicile *newDomicile = new Domicile(date, street, city);

                // Insert in sorted order by date
                Domicile **ptr = &c->domiciles;
                while (*ptr && strcmp((*ptr)->date, date) < 0) {
                    ptr = &((*ptr)->next);
                }
                newDomicile->next = *ptr;
                *ptr = newDomicile;

                return true;
            }
        }
        return false;
    }

    bool print(std::ostream &os, const char *id) const {
        for (Citizen *c = head; c; c = c->next) {
            if (strcmp(c->id, id) == 0) {
                os << c->id << " " << c->name << " " << c->surname << "\n";
                for (Domicile *d = c->domiciles; d; d = d->next) {
                    os << d->date << " " << d->street << " " << d->city << "\n";
                }
                return true;
            }
        }
        return false;
    }
};

#ifndef __PROGTEST__
int main ()
{
  char   lID[12], lDate[12], lName[50], lSurname[50], lStreet[50], lCity[50];
  std::ostringstream oss;
  CRegister  a;
  assert ( a . add ( "123456/7890", "John", "Smith", "2000-01-01", "Main street", "Seattle" ) == true );
  assert ( a . add ( "987654/3210", "Freddy", "Kruger", "2001-02-03", "Elm street", "Sacramento" ) == true );
  assert ( a . resettle ( "123456/7890", "2003-05-12", "Elm street", "Atlanta" ) == true );
  assert ( a . resettle ( "123456/7890", "2002-12-05", "Sunset boulevard", "Los Angeles" ) == true );
  oss . str ( "" );
  assert ( a . print ( oss, "123456/7890" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(123456/7890 John Smith
2000-01-01 Main street Seattle
2002-12-05 Sunset boulevard Los Angeles
2003-05-12 Elm street Atlanta
)###" ) );
  oss . str ( "" );
  assert ( a . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
)###" ) );
  CRegister b ( a );
  assert ( b . resettle ( "987654/3210", "2008-04-12", "Elm street", "Cinccinati" ) == true );
  assert ( a . resettle ( "987654/3210", "2007-02-11", "Elm street", "Indianapolis" ) == true );
  oss . str ( "" );
  assert ( a . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2007-02-11 Elm street Indianapolis
)###" ) );
  oss . str ( "" );
  assert ( b . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
)###" ) );
  a = b;
  assert ( a . resettle ( "987654/3210", "2011-05-05", "Elm street", "Salt Lake City" ) == true );
  oss . str ( "" );
  assert ( a . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
2011-05-05 Elm street Salt Lake City
)###" ) );
  oss . str ( "" );
  assert ( b . print ( oss, "987654/3210" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
)###" ) );
  assert ( b . add ( "987654/3210", "Joe", "Lee", "2010-03-17", "Abbey road", "London" ) == false );
  assert ( a . resettle ( "987654/3210", "2001-02-03", "Second street", "Milwaukee" ) == false );
  oss . str ( "" );
  assert ( a . print ( oss, "666666/6666" ) == false );

  CRegister  c;
  strncpy ( lID, "123456/7890", sizeof ( lID ) );
  strncpy ( lName, "John", sizeof ( lName ) );
  strncpy ( lSurname, "Smith", sizeof ( lSurname ) );
  strncpy ( lDate, "2000-01-01", sizeof ( lDate) );
  strncpy ( lStreet, "Main street", sizeof ( lStreet ) );
  strncpy ( lCity, "Seattle", sizeof ( lCity ) );
  assert (  c . add ( lID, lName, lSurname, lDate, lStreet, lCity ) == true );
  strncpy ( lID, "987654/3210", sizeof ( lID ) );
  strncpy ( lName, "Freddy", sizeof ( lName ) );
  strncpy ( lSurname, "Kruger", sizeof ( lSurname ) );
  strncpy ( lDate, "2001-02-03", sizeof ( lDate) );
  strncpy ( lStreet, "Elm street", sizeof ( lStreet ) );
  strncpy ( lCity, "Sacramento", sizeof ( lCity ) );
  assert (  c . add ( lID, lName, lSurname, lDate, lStreet, lCity ) == true );
  strncpy ( lID, "123456/7890", sizeof ( lID ) );
  strncpy ( lDate, "2003-05-12", sizeof ( lDate) );
  strncpy ( lStreet, "Elm street", sizeof ( lStreet ) );
  strncpy ( lCity, "Atlanta", sizeof ( lCity ) );
  assert ( c . resettle ( lID, lDate, lStreet, lCity ) == true );
  strncpy ( lID, "123456/7890", sizeof ( lID ) );
  strncpy ( lDate, "2002-12-05", sizeof ( lDate) );
  strncpy ( lStreet, "Sunset boulevard", sizeof ( lStreet ) );
  strncpy ( lCity, "Los Angeles", sizeof ( lCity ) );
  assert ( c . resettle ( lID, lDate, lStreet, lCity ) == true );
  oss . str ( "" );
  assert ( c . print ( oss, "123456/7890" ) == true );
  assert ( ! strcmp ( oss . str () . c_str (), R"###(123456/7890 John Smith
2000-01-01 Main street Seattle
2002-12-05 Sunset boulevard Los Angeles
2003-05-12 Elm street Atlanta
)###" ) );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
