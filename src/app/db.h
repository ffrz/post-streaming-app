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
Location getLocationById(int id);
bool saveLocation(Location &data);
bool removeLocation(const Location &data);

Teachers getAllTeachersOrderById();
Teachers getAllTeachersOrderByName();
Teachers getAllTeachers(const QString& orderColumn);
Teacher getTeacherById(int id);
bool saveTeacher(Teacher &teacher);
bool removeTeacher(const Teacher &teacher);

Books getAllBooksOrderById();
Books getAllBooksOrderByTitle();
Books getAllBooks(const QString& orderColumn);
Book getBookById(int id);
bool saveBook(Book &book);
bool removeBook(const Book &book);

StudyTextTemplates getAllStudyTextTemplatesOrderById();
StudyTextTemplates getAllStudyTextTemplatesOrderByName();
StudyTextTemplates getAllStudyTextTemplates(const QString& orderColumn);
StudyTextTemplate getStudyTextTemplateById(int id);
bool saveStudyTextTemplate(StudyTextTemplate &data);
bool removeStudyTextTemplate(const StudyTextTemplate &data);

}

#define DB_EXEC(Q) db::exec(Q, __FILE__, __LINE__)

#endif // DB_H
