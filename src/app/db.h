#ifndef DB_H
#define DB_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "data.h"

namespace db {

QSqlDatabase db();
void init();
bool exec(QSqlQuery &q, const QString& file, int line);

Locations getAllLocationsOrderById();
Locations getAllLocationsOrderByName();
Locations getAllLocations(const QString& orderColumn);
bool saveLocation(Location &data);
bool removeLocation(const Location &data);
Location getLocationById(int id);

Teachers getAllTeachersOrderById();
Teachers getAllTeachersOrderByName();
Teachers getAllTeachers(const QString& orderColumn);
bool saveTeacher(Teacher &teacher);
bool removeTeacher(const Teacher &teacher);
Teacher getTeacherById(int id);

Books getAllBooksOrderById();
Books getAllBooksOrderByTitle();
Books getAllBooks(const QString& orderColumn);
bool saveBook(Book &book);
bool removeBook(const Book &book);
Book getBookById(int id);

}

#define DB_EXEC(Q) db::exec(Q, __FILE__, __LINE__)

#endif // DB_H
