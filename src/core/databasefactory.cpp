#include <QApplication>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include "core/databasefactory.h"


QPointer<DatabaseFactory> DatabaseFactory::s_instance;

DatabaseFactory::DatabaseFactory(QObject *parent) : QObject(parent) {
  assemblyDatabaseFilePath();
}

DatabaseFactory::~DatabaseFactory() {
  qDebug("Destroying DatabaseFactory object.");
}

DatabaseFactory *DatabaseFactory::getInstance() {
  if (s_instance.isNull()) {
    s_instance = new DatabaseFactory(qApp);
  }

  return s_instance;
}

void DatabaseFactory::assemblyDatabaseFilePath()  {
  // TODO: Fill m_databasePath with correct path (portable or non-portable).
}

QString DatabaseFactory::getDatabasePath() {
  return m_databasePath;
}

QSqlDatabase DatabaseFactory::initialize(const QString &connection_name) {
  // Prepare file paths.
  QDir db_path(getDatabasePath());
  QFile db_file(db_path.absoluteFilePath("database.db"));

  // Check if database directory exists.
  if (!db_path.exists()) {
    if (!db_path.mkpath(db_path.absolutePath())) {
      // Failure when create database file path.
      qFatal("Directory for database file '%s' was NOT created."
             "This is HUGE problem.",
             qPrintable(db_file.symLinkTarget()));
    }
  }

  // Folders are created. Create new QSQLDatabase object.
  QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE", connection_name);

  // Setup database file path.
  database.setDatabaseName(db_file.symLinkTarget());

  if (!database.open()) {
    qFatal("Database was NOT opened. Delivered error message: '%s'",
           qPrintable(database.lastError().databaseText()));
  }
  else {
    database.exec("PRAGMA synchronous = OFF");
    database.exec("PRAGMA journal_mode = MEMORY");
    database.exec("PRAGMA count_changes = OFF");
    database.exec("PRAGMA temp_store = MEMORY");
    //database.exec("PRAGMA foreign_keys = ON");

    QSqlQuery q = database.exec("SELECT value FROM rssg_information WHERE key = 'schema_version'");

    if (q.lastError().isValid()) {
      qWarning("Error occurred. Database is not initialized. Initializing now.");

      QFile file_init(":/database/init.sql");
      file_init.open(QIODevice::ReadOnly | QIODevice::Text);

      QStringList statements = QString(file_init.readAll()).split("-- !\n");//--\n");
      database.exec("begin transaction");

      foreach(QString i, statements) {
        q = database.exec(i);
        if (q.lastError().isValid()) {
          if (q.lastError().number() != -1) {
            break;
          }
        }
      }

      database.exec("commit");
      qWarning("Database backend should be ready now.");
    }
    else {
      q.next();
      qDebug("Database connection '%s' to file %s seems to be loaded.",
             qPrintable(connection_name),
             qPrintable(QDir::toNativeSeparators(database.databaseName())));
      qDebug("Database has version %s.", qPrintable(q.value(0).toString()));
    }
    q.finish();
  }

  return database;
}

QSqlDatabase DatabaseFactory::addConnection(const QString &connection_name) {
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connection_name);

  if (!m_initialized) {
    return initialize(connection_name);
  }
  else {
    return QSqlDatabase::addDatabase("QSQLITE", connection_name);
  }
}

QSqlDatabase DatabaseFactory::getConnection(const QString &connection_name) {
  return QSqlDatabase::database(connection_name);
}

void DatabaseFactory::removeConnection(const QString &connection_name) {
  qDebug("Removing database connection '%s'.", qPrintable(connection_name));
  QSqlDatabase::removeDatabase(connection_name);
}
