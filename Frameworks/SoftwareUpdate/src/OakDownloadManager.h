NS_ASSUME_NONNULL_BEGIN

@interface OakDownloadManager : NSObject
@property (class, readonly) OakDownloadManager* sharedInstance;
@property (nonatomic) NSString* userAgentString;
- (void)downloadFileAtURL:(NSURL*)serverURL replacingFileAtURL:(NSURL*)localFileURL publicKeys:(NSDictionary<NSString*, NSString*>*)publicKeys completionHandler:(void(^)(BOOL wasUpdated, NSError* error))completionHandler;
// Downloads serverURL and serverURL + ".sig", and replaces localFileURL when the
// signature verifies with the release key (signing::kReleasePublicKey).
- (void)downloadSignedFileAtURL:(NSURL*)serverURL replacingFileAtURL:(NSURL*)localFileURL completionHandler:(void(^)(BOOL wasUpdated, NSError* error))completionHandler;
// Downloads and extracts an archive whose SHA-256 digest (lowercase hexadecimal) is sha256.
- (id <NSProgressReporting>)downloadArchiveAtURL:(NSURL*)serverURL forReplacingURL:(nullable NSURL*)localURL sha256:(NSString*)sha256 completionHandler:(void(^)(NSURL* extractedArchiveURL, NSError* error))completionHandler;
- (id <NSProgressReporting>)downloadArchiveAtURL:(NSURL*)serverURL forReplacingURL:(nullable NSURL*)localURL publicKeys:(NSDictionary<NSString*, NSString*>*)publicKeys completionHandler:(void(^)(NSURL* extractedArchiveURL, NSError* error))completionHandler;
@end

NS_ASSUME_NONNULL_END
