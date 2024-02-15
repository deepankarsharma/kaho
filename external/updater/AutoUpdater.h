/*
 * Copyright (C) 2008 Remko Troncon
 * Copyright (C) 2017 Vadim Peretokin
 */

#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

class AutoUpdater
{
public:
    virtual ~AutoUpdater();

    virtual void checkForUpdates() = 0;

    virtual void setAutomaticallyChecksForUpdates(bool on) = 0;
    virtual bool automaticallyChecksForUpdates() = 0;

    virtual void setAutomaticallyDownloadsUpdates(bool on) = 0;
    virtual bool automaticallyDownloadsUpdates() = 0;

    virtual bool justUpdated() = 0;
};

#endif
