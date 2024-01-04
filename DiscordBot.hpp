#pragma once
#include <string>

namespace Discord_Tools {
	void discord_enqueue_message(uint64_t channel_id, const std::string& message);
	void discord_messenger();
}