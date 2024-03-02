#pragma once
#include <QtGlobal>
#include <QString>
#include <QObject>

namespace kaho {
class AutoUpdater : public QObject{
  Q_OBJECT
 public:
  ~AutoUpdater() override = default;
  virtual void checkForUpdates() = 0;
 signals:
  // Emitted by sparkle when the state of canCheckForUpdates changes
  // It is not safe to check for updates while an update is in progress
  void canCheckForUpdatesChanged(bool canCheck);
};


class Updater : public AutoUpdater {
 public:
  Updater();
  ~Updater() override;
  void checkForUpdates() override;

 private:
  class Private;
  Private* d;
  bool relaunchedFromUpdate;
};



}  // namespace kaho