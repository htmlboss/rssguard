// This file is part of RSS Guard.
//
// Copyright (C) 2011-2014 by Martin Rotter <rotter.martinos@gmail.com>
//
// RSS Guard is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RSS Guard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RSS Guard. If not, see <http://www.gnu.org/licenses/>.

#include "gui/formrestoredatabasesettings.h"

#include "miscellaneous/iconfactory.h"

#include "QFileDialog"


FormRestoreDatabaseSettings::FormRestoreDatabaseSettings(QWidget *parent)
  : QDialog(parent), m_ui(new Ui::FormRestoreDatabaseSettings) {
  m_ui->setupUi(this);

  setWindowIcon(qApp->icons()->fromTheme("document-import"));
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog | Qt::WindowSystemMenuHint);

  m_ui->m_lblResult->setStatus(WidgetWithStatus::Warning, tr("No operation executed yet."), tr("No operation executed yet."));

  connect(m_ui->m_btnSelectFolder, SIGNAL(clicked()), this, SLOT(selectFolder()));
  connect(m_ui->m_groupDatabase, SIGNAL(toggled(bool)), this, SLOT(checkOkButton()));
  connect(m_ui->m_groupSettings, SIGNAL(toggled(bool)), this, SLOT(checkOkButton()));
  connect(m_ui->m_buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(performRestoration()));

  selectFolder(qApp->documentsFolderPath());
}

FormRestoreDatabaseSettings::~FormRestoreDatabaseSettings() {
  delete m_ui;
}

void FormRestoreDatabaseSettings::performRestoration() {
  // TODO: Pokračovat
}

void FormRestoreDatabaseSettings::checkOkButton() {
  m_ui->m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!m_ui->m_lblSelectFolder->label()->text().isEmpty() &&
                                                              ((m_ui->m_groupDatabase->isChecked() &&
                                                                m_ui->m_listDatabase->currentRow() >= 0) ||
                                                               (m_ui->m_groupSettings->isChecked() &&
                                                                m_ui->m_listSettings->currentRow() >= 0)));
}

void FormRestoreDatabaseSettings::selectFolder(QString folder) {
  if (folder.isEmpty()) {
    folder = QFileDialog::getExistingDirectory(this, tr("Select source folder"), m_ui->m_lblSelectFolder->label()->text());
  }

  if (!folder.isEmpty()) {
    m_ui->m_lblSelectFolder->setStatus(WidgetWithStatus::Ok, QDir::toNativeSeparators(folder),
                                       tr("Good source folder is specified."));
  }
  else {
    return;
  }

  QDir selected_folder(folder);
  QFileInfoList available_databases = selected_folder.entryInfoList(QStringList() << QString("*") + BACKUP_SUFFIX_DATABASE ,
                                                                    QDir::Files | QDir::NoDotAndDotDot | QDir::Readable |
                                                                    QDir::CaseSensitive | QDir::NoSymLinks,
                                                                    QDir::Name);
  QFileInfoList available_settings = selected_folder.entryInfoList(QStringList() << QString("*") + BACKUP_SUFFIX_SETTINGS ,
                                                                   QDir::Files | QDir::NoDotAndDotDot | QDir::Readable |
                                                                   QDir::CaseSensitive | QDir::NoSymLinks,
                                                                   QDir::Name);

  m_ui->m_listDatabase->clear();
  m_ui->m_listSettings->clear();

  foreach (const QFileInfo &database_file, available_databases) {
    QListWidgetItem *database_item = new QListWidgetItem(database_file.fileName(), m_ui->m_listDatabase);
    database_item->setData(Qt::UserRole, database_file.absoluteFilePath());
    database_item->setToolTip(QDir::toNativeSeparators(database_file.absoluteFilePath()));
  }

  foreach (const QFileInfo &settings_file, available_settings) {
    QListWidgetItem *settings_item = new QListWidgetItem(settings_file.fileName(), m_ui->m_listSettings);
    settings_item->setData(Qt::UserRole, settings_file.absoluteFilePath());
    settings_item->setToolTip(QDir::toNativeSeparators(settings_file.absoluteFilePath()));
  }

  if (!available_databases.isEmpty()) {
    m_ui->m_listDatabase->setCurrentRow(0);
  }

  if (!available_settings.isEmpty()) {
    m_ui->m_listSettings->setCurrentRow(0);
  }

  m_ui->m_groupDatabase->setChecked(!available_databases.isEmpty());
  m_ui->m_groupSettings->setChecked(!available_settings.isEmpty());
}
