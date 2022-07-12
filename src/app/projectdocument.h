#ifndef PROJECTDOCUMENT_H
#define PROJECTDOCUMENT_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

class ProjectDocument
{
public:
    static const char *DOC_KEY_TYPE;
    static const char *DOC_KEY_VERSION;
    static const char *DOC_KEY_DATA;

    static const char *DATA_KEY_STUDY_TYPE;
    static const char *DATA_KEY_TITLE;
    static const char *DATA_KEY_SUBTITLE;
    static const char *DATA_KEY_AUTHOR;
    static const char *DATA_KEY_NARRATOR;
    static const char *DATA_KEY_STUDY_DESCRIPTION;
    static const char *DATA_KEY_TEACHER;
    static const char *DATA_KEY_TEACHER_DETAILS;
    static const char *DATA_KEY_STUDY_URL;

    static const char *DATA_KEY_DATE;
    static const char *DATA_KEY_HIJRI_DATE;
    static const char *DATA_KEY_START_TIME;
    static const char *DATA_KEY_END_TIME;
    static const char *DATA_KEY_LOCATION_NAME;
    static const char *DATA_KEY_LOCATION_ADDRESS;
    static const char *DATA_KEY_LOCATION_URL;

    ProjectDocument(const QString &fileName = QString());

    static ProjectDocument *open(const QString & fileName);

    void setData(const QJsonObject &data);
    QJsonObject data() const;

    QString fileName() const;
    void setFileName(const QString &fileName);

    void initialize();
    bool save();

private:
    QString _fileName;
    QJsonDocument _jsonDoc;
};

#endif // PROJECTDOCUMENT_H
