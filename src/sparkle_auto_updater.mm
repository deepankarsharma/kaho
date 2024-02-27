#include <kaho/updater.h>

#include <Cocoa/Cocoa.h>
#include <Sparkle/Sparkle.h>

@interface UpdaterObserver : NSObject

@property(nonatomic, readonly) SPUUpdater* updater;

/// A callback to run when the state of `canCheckForUpdates` for the `updater` changes.
@property(nonatomic, copy) void (^callback) (bool);

- (id)initWithUpdater:(SPUUpdater*)updater;

@end

@implementation UpdaterObserver

- (id)initWithUpdater:(SPUUpdater*)updater
{
    self = [super init];
    _updater = updater;
    [self addObserver:self forKeyPath:@"updater.canCheckForUpdates" options:NSKeyValueObservingOptionNew context:nil];

    return self;
}

- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary<NSKeyValueChangeKey, id> *)change
                       context:(void *)context
{
    if ([keyPath isEqualToString:@"updater.canCheckForUpdates"])
    {
        bool canCheck = [change[NSKeyValueChangeNewKey] boolValue];
        self.callback(canCheck);
    }
}

@end


@interface UpdaterDelegate : NSObject <SPUUpdaterDelegate>

@property(nonatomic, copy) NSSet<NSString *> *allowedChannels;

@end

@implementation UpdaterDelegate

- (NSSet<NSString *> *)allowedChannelsForUpdater:(SPUUpdater *)updater
{
    return _allowedChannels;
}

@end


class kaho::SparkleAutoUpdater::Private
{
public:
    SPUStandardUpdaterController *updaterController;
    UpdaterObserver *updaterObserver;
    UpdaterDelegate *updaterDelegate;
    NSAutoreleasePool *autoReleasePool;
};

kaho::SparkleAutoUpdater::SparkleAutoUpdater()
{
    d = new SparkleAutoUpdater::Private();

    // Enable Cocoa's memory management.
    NSApplicationLoad();
    d->autoReleasePool = [[NSAutoreleasePool alloc] init];
    d->updaterDelegate = [[UpdaterDelegate alloc] init];
    d->updaterController = [[SPUStandardUpdaterController alloc] initWithStartingUpdater:true
                               updaterDelegate:d->updaterDelegate
                               userDriverDelegate:nil];
    d->updaterObserver = [[UpdaterObserver alloc] initWithUpdater:d->updaterController.updater];
    d->updaterObserver.callback = ^(bool canCheck) {
        emit canCheckForUpdatesChanged(canCheck);
    };
}

kaho::SparkleAutoUpdater::~SparkleAutoUpdater()
{
    [d->updaterObserver removeObserver:d->updaterObserver forKeyPath:@"updater.canCheckForUpdates"];

    [d->updaterController release];
    [d->updaterObserver release];
    [d->updaterDelegate release];
    [d->autoReleasePool release];
    delete d;
}

void kaho::SparkleAutoUpdater::checkForUpdates()
{
    [d->updaterController checkForUpdates:nil];
}





