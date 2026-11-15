#ifndef UPDATER_DOWNLOAD_H_842XT36M
#define UPDATER_DOWNLOAD_H_842XT36M

#include <oak/oak.h>

namespace bundles_db
{
	// Downloads url and url + ".sig", and returns the path of the downloaded
	// file when the signature verifies with the release key. Returns NULL_STR
	// when the file is unchanged (etag) or cannot be verified.
	std::string download_etag (std::string const& url, std::string* etag, double* progress, double min, double max);

} /* bundles_db */

#endif /* end of include guard: UPDATER_DOWNLOAD_H_842XT36M */
