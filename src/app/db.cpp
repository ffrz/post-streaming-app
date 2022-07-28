#include "db.h"
#include "global.h"

#include <QMessageBox>

namespace db {

QSqlDatabase db()
{
    return QSqlDatabase::database();
}

void init() {
    // Inisialisasi database
    // File database akan buat secara otomatis jika file database tidak tersedia
    QSqlDatabase db = QSqlDatabase::addDatabase(DB_DRIVER);
    db.setDatabaseName(DB_FILENAME);
    db.open();

    QSqlQuery q(db);
    q.exec("CREATE TABLE IF NOT EXISTS teachers ("
           " id             INTEGER NOT NULL DEFAULT 0,"
           " name           TEXT NOT NULL DEFAULT '',"
           " description	TEXT NOT NULL DEFAULT '',"
           " PRIMARY KEY(id AUTOINCREMENT)"
           ")");
    q.exec("CREATE TABLE IF NOT EXISTS locations ("
           " id         INTEGER NOT NULL DEFAULT 0,"
           " name       TEXT NOT NULL DEFAULT '',"
           " address	TEXT NOT NULL DEFAULT '',"
           " url        INTEGER NOT NULL DEFAULT '',"
           " PRIMARY KEY(id AUTOINCREMENT)"
           ")");
    q.exec("CREATE TABLE IF NOT EXISTS books("
           " id         INTEGER NOT NULL DEFAULT 0,"
           " title      TEXT NOT NULL DEFAULT '',"
           " subtitle 	TEXT NOT NULL DEFAULT '',"
           " authors 	TEXT NOT NULL DEFAULT '',"
           " narrator 	TEXT NOT NULL DEFAULT '',"
           " PRIMARY KEY(id AUTOINCREMENT)"
           ")");
    q.exec("CREATE TABLE IF NOT EXISTS study_text_templates ("
           " id         INTEGER NOT NULL DEFAULT 0,"
           " name       TEXT NOT NULL DEFAULT '',"
           " content    TEXT NOT NULL DEFAULT '',"
           " PRIMARY KEY(id AUTOINCREMENT)"
           ")");
    q.exec("CREATE TABLE IF NOT EXISTS audio_text_templates ("
           " id         INTEGER NOT NULL DEFAULT 0,"
           " name       TEXT NOT NULL DEFAULT '',"
           " content    TEXT NOT NULL DEFAULT '',"
           " PRIMARY KEY(id AUTOINCREMENT)"
           ")");
}

bool exec(QSqlQuery &q, const QString& file, int line)
{
    if (!q.exec()) {
        /// TODO: catat ke log
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle("Sql Error");
        msgBox.setText("Terdapat kesalahan saat mengeksekusi query.<br>");
        msgBox.setInformativeText(QString("file: %2:%3<br>")
                                  .arg(file, QString("%1").arg(line)));
        msgBox.setDetailedText(QString("SQL: '%1'. "
                                       "Database error text: %2")
                               .arg(q.lastQuery())
                               .arg(q.lastError().text()));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();

        return false;
    }

    return true;
}

Locations getAllLocationsOrderById()
{
    return getAllLocations("id");
}

Locations getAllLocationsOrderByName()
{
    return getAllLocations("name");
}

Locations getAllLocations(const QString& col)
{
    Locations items;
    QSqlQuery q(db());
    q.prepare("select * from locations order by " + col + " asc");
    DB_EXEC(q);
    while (q.next()) {
        Location item;
        item.id = q.value("id").toInt();
        item.name = q.value("name").toString();
        item.address = q.value("address").toString();
        item.url = q.value("url").toString();
        items.append(item);
    }
    return items;
}

Location getLocationById(int id)
{
    Location d;
    QSqlQuery q(db());
    q.prepare("select * from locations where id=:id");
    q.bindValue(":id", id);
    if (DB_EXEC(q) && q.next()) {
        d.id = q.value("id").toInt();
        d.name = q.value("name").toString();
        d.address = q.value("address").toString();
        d.url = q.value("url").toString();
    }
    return d;
}

bool removeLocation(const Location &d)
{
    QSqlQuery q(db());
    q.prepare("delete from locations where id=:id");
    q.bindValue(":id", d.id);
    return DB_EXEC(q);
}

bool saveLocation(Location &d)
{
    QSqlQuery q(db());
    if (d.id == 0) {
        q.prepare("insert into locations"
                  " (name, address, url)"
                  " values"
                  " (:name,:address,:url)");
    }
    else {
        q.prepare("update locations set"
                  " name=:name,"
                  " address=:address,"
                  " url=:url"
                  " where id=:id");
        q.bindValue(":id", d.id);
    }

    q.bindValue(":name", d.name);
    q.bindValue(":address", d.address);
    q.bindValue(":url", d.url);

    if (!DB_EXEC(q))
        return false;

    if (d.id == 0)
        d.id = q.lastInsertId().toInt();

    return true;
}

Teachers getAllTeachersOrderById()
{
    return getAllTeachers("id");
}

Teachers getAllTeachersOrderByName()
{
    return getAllTeachers("name");
}

Teachers getAllTeachers(const QString& col)
{
    Teachers items;
    QSqlQuery q(db());
    q.prepare("select id, name, description from teachers order by " + col + " asc");
    DB_EXEC(q);
    while (q.next()) {
        Teacher d;
        d.id = q.value("id").toInt();
        d.name = q.value("name").toString();
        d.description = q.value("description").toString();
        items.append(d);
    }
    return items;
}

Teacher getTeacherById(int id)
{
    Teacher d;
    QSqlQuery q(db());
    q.prepare("select id, name, description from teachers where id=:id");
    q.bindValue(":id", id);
    if (DB_EXEC(q) && q.next()) {
        d.id = q.value("id").toInt();
        d.name = q.value("name").toString();
        d.description = q.value("description").toString();
    }
    return d;
}

bool saveTeacher(Teacher &d)
{
    QSqlQuery q(db());
    if (d.id == 0) {
        q.prepare("insert into teachers"
                  " (name, description)"
                  " values"
                  " (:name,:description)");
    }
    else {
        q.prepare("update teachers set"
                  " name=:name,"
                  " description=:description"
                  " where id=:id");
        q.bindValue(":id", d.id);
    }
    q.bindValue(":name", d.name);
    q.bindValue(":description", d.description);

    if (!DB_EXEC(q))
        return false;

    if (d.id == 0)
        d.id = q.lastInsertId().toInt();

    return true;
}

bool removeTeacher(const Teacher &teacher)
{
    QSqlQuery q(db());
    q.prepare("delete from teachers where id=:id");
    q.bindValue(":id", teacher.id);
    return DB_EXEC(q);
}

Books getAllBooksOrderById()
{
    return getAllBooks("id");
}

Books getAllBooksOrderByTitle()
{
    return getAllBooks("title");
}

Books getAllBooks(const QString& col)
{
    Books items;
    QSqlQuery q(db());
    q.prepare("select * from books order by " + col + " asc");
    DB_EXEC(q);
    while (q.next()) {
        Book item;
        item.id = q.value("id").toInt();
        item.title = q.value("title").toString();
        item.subtitle = q.value("subtitle").toString();
        item.authors = q.value("authors").toString();
        item.narrator = q.value("narrator").toString();
        items.append(item);
    }
    return items;
}

Book getBookById(int id)
{
    Book d;
    QSqlQuery q(db());
    q.prepare("select * from books where id=:id");
    q.bindValue(":id", id);
    if (DB_EXEC(q) && q.next()) {
        d.id = q.value("id").toInt();
        d.title = q.value("title").toString();
        d.subtitle = q.value("subtitle").toString();
        d.authors = q.value("authors").toString();
        d.narrator = q.value("narrator").toString();
    }
    return d;
}

bool saveBook(Book &item)
{
    QSqlQuery q(db());
    if (item.id == 0) {
        q.prepare("insert into books"
                  " (title, subtitle, authors, narrator)"
                  " values"
                  " (:title,:subtitle,:authors,:narrator)");
    }
    else {
        q.prepare("update books set"
                  " title=:title,"
                  " subtitle=:subtitle,"
                  " authors=:authors,"
                  " narrator=:narrator"
                  " where id=:id");
        q.bindValue(":id", item.id);
    }
    q.bindValue(":title", item.title);
    q.bindValue(":subtitle", item.subtitle);
    q.bindValue(":authors", item.authors);
    q.bindValue(":narrator", item.narrator);

    if (!DB_EXEC(q))
        return false;

    if (item.id == 0)
        item.id = q.lastInsertId().toInt();

    return true;
}

bool removeBook(const Book &book)
{
    QSqlQuery q(db());
    q.prepare("delete from books where id=:id");
    q.bindValue(":id", book.id);
    return DB_EXEC(q);
}

StudyTextTemplates getAllStudyTextTemplatesOrderById()
{
    return getAllStudyTextTemplates("id");
}

StudyTextTemplates getAllStudyTextTemplatesOrderByName()
{
    return getAllStudyTextTemplates("name");
}

StudyTextTemplates getAllStudyTextTemplates(const QString& col)
{
    StudyTextTemplates items;
    QSqlQuery q(db());
    q.prepare("select id, name, content from study_text_templates order by " + col + " asc");
    DB_EXEC(q);
    while (q.next()) {
        StudyTextTemplate d;
        d.id = q.value("id").toInt();
        d.name = q.value("name").toString();
        d.content = q.value("content").toString();
        items.append(d);
    }
    return items;
}

StudyTextTemplate getStudyTextTemplateById(int id)
{
    StudyTextTemplate d;
    QSqlQuery q(db());
    q.prepare("select id, name, content from study_text_templates where id=:id");
    q.bindValue(":id", id);
    if (DB_EXEC(q) && q.next()) {
        d.id = q.value("id").toInt();
        d.name = q.value("name").toString();
        d.content = q.value("content").toString();
    }
    return d;
}

bool saveStudyTextTemplate(StudyTextTemplate &d)
{
    QSqlQuery q(db());
    if (d.id == 0) {
        q.prepare("insert into study_text_templates"
                  " (name, content)"
                  " values"
                  " (:name,:content)");
    }
    else {
        q.prepare("update study_text_templates set"
                  " name=:name,"
                  " content=:content"
                  " where id=:id");
        q.bindValue(":id", d.id);
    }
    q.bindValue(":name", d.name);
    q.bindValue(":content", d.content);

    if (!DB_EXEC(q))
        return false;

    if (d.id == 0)
        d.id = q.lastInsertId().toInt();

    return true;
}

bool removeStudyTextTemplate(const StudyTextTemplate &d)
{
    QSqlQuery q(db());
    q.prepare("delete from study_text_templates where id=:id");
    q.bindValue(":id", d.id);
    return DB_EXEC(q);
}

}
