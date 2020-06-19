// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MESSAGEFILTER_H
#define MESSAGEFILTER_H

#include <QObject>

#include "core/message.h"

// Class which represents one message filter.
class MessageFilter : public QObject {
  Q_OBJECT

  public:
    explicit MessageFilter(QObject* parent = nullptr);

    FilteringAction filterMessage();

  private:
    int m_id;
    QString m_name;
    QString m_script;
};

#endif // MESSAGEFILTER_H