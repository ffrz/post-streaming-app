#ifndef DATA_H
#define DATA_H

#include <QString>

struct Location
{
    int id;
    QString name;
    QString address;
    QString url;
    Location() : id(0) {}
};

struct Teacher
{
    int id;
    QString name;
    QString description;
    Teacher() : id(0) {}
};

struct Book
{
    int id;
    QString title;
    QString subtitle;
    QString authors;
    QString narrator;
    Book() : id(0) {}
};

typedef QList<Teacher> Teachers;
typedef QList<Book> Books;
typedef QList<Location> Locations;

#endif // DATA_H
