#include "download.h"
#include <network/network.h>
#include <signing/signing.h>
#include <io/path.h>

namespace
{
	struct collect_t : filter_t
	{
		bool receive_data (char const* bytes, size_t len) { data.insert(data.end(), bytes, bytes + len); return true; }
		std::string name ()                               { return "collect"; }
		std::string data;
	};
}

namespace bundles_db
{
	std::string download_etag (std::string const& url, std::string* etag, double* progress, double min, double max)
	{
		collect_t content;
		network::save_t archiver(false);
		network::header_t collect_etag("etag");

		std::string error = NULL_STR;
		long res = network::download(network::request_t(url, &content, &archiver, &collect_etag, nullptr).set_entity_tag(etag ? *etag : NULL_STR).update_progress_variable(progress, min, max), &error);
		if(res == 304) // not modified
		{
			path::remove(archiver.path);
		}
		else if(res == 200)
		{
			collect_t signature;
			std::string sigError = NULL_STR;
			long sigRes = network::download(network::request_t(url + ".sig", &signature, nullptr), &sigError);
			if(sigRes == 200 && signing::verify(content.data, signature.data))
			{
				if(etag)
					*etag = collect_etag.value();
				return archiver.path;
			}

			path::remove(archiver.path);
			if(sigRes == 200)
					os_log_error(OS_LOG_DEFAULT, "download_etag(‘%{public}s’): bad signature", url.c_str());
			else	os_log_error(OS_LOG_DEFAULT, "download_etag(‘%{public}s.sig’): %{public}s", url.c_str(), sigRes ? std::to_string(sigRes).c_str() : sigError.c_str());
		}
		else
		{
			if(res != 0)
					os_log_error(OS_LOG_DEFAULT, "download_etag(‘%{public}s’): got ‘%ld’ from server (expected 200)", url.c_str(), res);
			else	os_log_error(OS_LOG_DEFAULT, "download_etag(‘%{public}s’): %{public}s", url.c_str(), error.c_str());
		}
		return NULL_STR;
	}
}
