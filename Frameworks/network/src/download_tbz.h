#ifndef NETWORK_DOWNLOAD_TBZ_H_MYCOASC1
#define NETWORK_DOWNLOAD_TBZ_H_MYCOASC1

#include <string>

namespace network
{
	// Downloads and extracts the archive at url, after checking that its
	// SHA-256 digest is expectedSHA256 (lowercase hexadecimal).
	std::string download_tbz (std::string const& url, std::string const& expectedSHA256, std::string const& destination, std::string& error, double* progress, double progressStart = 0, double progressStop = 1, bool const* stopFlag = NULL);

} /* network */

#endif /* end of include guard: NETWORK_DOWNLOAD_TBZ_H_MYCOASC1 */
