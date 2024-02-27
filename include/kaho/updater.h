#pragma once
#include <QString>
#include <QObject>

namespace kaho {
class AutoUpdater : public QObject{
  Q_OBJECT
 public:
  virtual ~AutoUpdater(){};
  virtual void checkForUpdates() = 0;
 signals:
  // Emitted by sparkle when the state of canCheckForUpdates changes
  // It is not safe to check for updates while an update is in progress
  void canCheckForUpdatesChanged(bool canCheck);
};

class SparkleAutoUpdater : public AutoUpdater {
 public:
  SparkleAutoUpdater();
  ~SparkleAutoUpdater() override;
  void checkForUpdates() override;

 private:
  class Private;
  Private* d;
  bool relaunchedFromUpdate;
};

}  // namespace kaho