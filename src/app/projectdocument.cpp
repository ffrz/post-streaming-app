#include "projectdocument.h"
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>

const char *ProjectDocument::DOC_KEY_TYPE = "doc-type";
const char *ProjectDocument::DOC_KEY_VERSION = "doc-version";
const char *ProjectDocument::DOC_KEY_DATA = "doc-data";

const char *ProjectDocument::DATA_KEY_STUDY_TYPE = "study-type";
const char *ProjectDocument::DATA_KEY_TITLE = "title";
const char *ProjectDocument::DATA_KEY_SUBTITLE = "subtitle";
const char *ProjectDocument::DATA_KEY_AUTHOR = "author";
const char *ProjectDocument::DATA_KEY_NARRATOR = "narrator";
const char *ProjectDocument::DATA_KEY_STUDY_DESCRIPTION = "study-description";
const char *ProjectDocument::DATA_KEY_TEACHER = "teacher";
const char *ProjectDocument::DATA_KEY_TEACHER_DETAILS= "teacher-details";
const char *ProjectDocument::DATA_KEY_STUDY_URL = "study-url";

const char *ProjectDocument::DATA_KEY_DATE = "date";
const char *ProjectDocument::DATA_KEY_HIJRI_DATE = "hijri-date";
const char *ProjectDocument::DATA_KEY_START_TIME = "start-time";
const char *ProjectDocument::DATA_KEY_END_TIME = "end-time";
const char *ProjectDocument::DATA_KEY_LOCATION_NAME = "loc-name";
const char *ProjectDocument::DATA_KEY_LOCATION_ADDRESS = "loc-address";
const char *ProjectDocument::DATA_KEY_LOCATION_URL = "loc-url";

ProjectDocument::ProjectDocument(const QString &fileName)
    : _fileName(fileName)
{
}

ProjectDocument *ProjectDocument::open(const QString & fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::Text | QFile::ReadOnly))
        return nullptr;

    ProjectDocument *doc = new ProjectDocument(fileName);
    doc->_jsonDoc = QJsonDocument::fromJson(file.readAll());
    return doc;
}

QString ProjectDocument::fileName() const
{
    return _fileName;
}

void ProjectDocument::setFileName(const QString &fileName)
{
    _fileName = fileName;
}

void ProjectDocument::initialize()
{
    QJsonObject root;
    QJsonObject data;

    data.insert(DATA_KEY_STUDY_TYPE, 0);
    data.insert(DATA_KEY_TITLE, "");
    data.insert(DATA_KEY_SUBTITLE, "");
    data.insert(DATA_KEY_STUDY_DESCRIPTION, "");
    data.insert(DATA_KEY_AUTHOR, "");
    data.insert(DATA_KEY_NARRATOR, "");
    data.insert(DATA_KEY_TEACHER, "");
    data.insert(DATA_KEY_TEACHER_DETAILS, "");
    data.insert(DATA_KEY_STUDY_URL, "");

    data.insert(DATA_KEY_DATE, "");
    data.insert(DATA_KEY_HIJRI_DATE, "");
    data.insert(DATA_KEY_START_TIME, "");
    data.insert(DATA_KEY_END_TIME, "");

    data.insert(DATA_KEY_LOCATION_NAME, "");
    data.insert(DATA_KEY_LOCATION_ADDRESS, "");
    data.insert(DATA_KEY_LOCATION_URL, "");

    root.insert(DOC_KEY_TYPE, "kpro");
    root.insert(DOC_KEY_VERSION, "1");
    root.insert(DOC_KEY_DATA, data);
    _jsonDoc.setObject(root);
}

void ProjectDocument::setData(const QJsonObject &data)
{
    QJsonObject root = _jsonDoc.object();
    root[DOC_KEY_DATA] = data;
    _jsonDoc.setObject(root);
}

QJsonObject ProjectDocument::data() const
{
    QJsonObject root = _jsonDoc.object();
    return root.value(DOC_KEY_DATA).toObject();
}

bool ProjectDocument::save()
{
    QFile file(_fileName);
    if (!file.open(QFile::Text | QFile::ReadWrite | QFile::Truncate)) {
        return false;
    }

    file.write(_jsonDoc.toJson());

    return file.error() == QFileDevice::NoError;
}
