/*
 * Copyright (C) 2008 Remko Troncon
 * Copyright (C) 2017 Vadim Peretokin
 */

#include "SparkleAutoUpdater.h"

#include <Cocoa/Cocoa.h>
#include <Sparkle/Sparkle.h>

#include <syslog.h>
static void SYSLOG(const char* format,...)
{
    va_list vaList;
    va_start( vaList,format );
    vsyslog(LOG_ERR,format,vaList);
    va_end(vaList);
}

class SparkleAutoUpdater::Private
{
public:
    SUUpdater* updater;
};

@interface SparkleDelegate : NSObject <SUUpdaterDelegate> {
  SparkleAutoUpdater* delegateHandler;
}

- (void)updaterDidRelaunchApplication:(SUUpdater *)updater;
@end

@implementation SparkleDelegate
- (id) init:(SparkleAutoUpdater*)handler
{
    self = [super init];
    if (self) {
        delegateHandler = handler;
    }
    return self;
}

- (void)updaterDidRelaunchApplication:(SUUpdater *)updater
{
    delegateHandler->setRelaunchFlag();
}
@end

void SparkleAutoUpdater::setRelaunchFlag()
{
    relaunchedFromUpdate = true;
}

bool SparkleAutoUpdater::justUpdated()
{
    return relaunchedFromUpdate;
}

SparkleAutoUpdater::SparkleAutoUpdater(const QString& aUrl)
{
    d = new Private;

    d->updater = [SUUpdater sharedUpdater];
    [d->updater retain];

    NSURL* url = [NSURL URLWithString:
    [NSString stringWithUTF8String: aUrl.toUtf8().data()]];
    [d->updater setFeedURL: url];

    relaunchedFromUpdate = false;
}

SparkleAutoUpdater::~SparkleAutoUpdater()
{
    [d->updater release];
    delete d;
}

void SparkleAutoUpdater::checkForUpdates()
{
    [d->updater checkForUpdates:nil];
}

void SparkleAutoUpdater::setAutomaticallyChecksForUpdates(bool on)
{
    [d->updater setAutomaticallyChecksForUpdates:on];
}

bool SparkleAutoUpdater::automaticallyChecksForUpdates()
{
    return [d->updater automaticallyChecksForUpdates];
}

void SparkleAutoUpdater::setAutomaticallyDownloadsUpdates(bool on)
{
    [d->updater setAutomaticallyDownloadsUpdates:on];
}

bool SparkleAutoUpdater::automaticallyDownloadsUpdates()
{
    return [d->updater automaticallyDownloadsUpdates];
}
