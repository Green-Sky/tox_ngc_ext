#include "./ngc_ext.hpp"

//#include <cstdint>
#include <cstdio>
#include <new>

#ifndef EXTRA_LOGGING
#define EXTRA_LOGGING 0
#endif

using namespace NGC_EXT;

// global scope

NGC_EXT_CTX* NGC_EXT_new(void) {
	auto* ctx = new NGC_EXT_CTX;
	// TODO: this is likely not necessary
	for (auto& it : ctx->callbacks) {
		it = nullptr;
	}
	for (auto& it : ctx->user_data) {
		it = nullptr;
	}
	return ctx;
}

void NGC_EXT_kill(NGC_EXT_CTX* ngc_ext_ctx) {
	delete ngc_ext_ctx;
}

#define _EXT_HAVE(x, error) if ((length - curser) < (x)) { error; }
void NGC_EXT_handle_group_custom_packet(
	Tox* tox,
	NGC_EXT_CTX* ngc_ext_ctx,

	uint32_t group_number,
	uint32_t peer_number,

	const uint8_t* data,
	size_t length
	//void *user_data
) {
	size_t curser = 0;

	_EXT_HAVE(1, return)

	PacketType pkg_type = static_cast<PacketType>(*(data + curser));
	curser++;

#if defined(EXTRA_LOGGING) && EXTRA_LOGGING == 1
	fprintf(stderr, "EX: custom_packet [%s] %lu\n", pkgid2str(pkg_type), length);
#endif

	if (pkg_type == INVALID) {
		fprintf(stderr, "(invalid)\n");
		return;
	}

	auto handle_fn = ngc_ext_ctx->callbacks[pkg_type];
	auto handle_ud = ngc_ext_ctx->user_data[pkg_type]; // can be null
	if (handle_fn == nullptr) {
#if defined(EXTRA_LOGGING) && EXTRA_LOGGING == 1
		fprintf(stderr, "EX: !!! no handler for packet\n");
#endif
		return;
	}

	handle_fn(tox, ngc_ext_ctx, group_number, peer_number, data+curser, length-curser, handle_ud);
}
#undef _EXT_HAVE

namespace NGC_EXT {

bool GroupKey::operator<(const GroupKey& rhs) const {
	for (size_t i = 0; i < data.size(); i++) {
		if (data[i] < rhs.data[i]) {
			return true;
		} else if (data[i] > rhs.data[i]) {
			return false;
		}
	}
	return false; // equal
}

bool GroupKey::operator==(const GroupKey& rhs) const {
	for (size_t i = 0; i < data.size(); i++) {
		if (data[i] != rhs.data[i]) {
			return false;
		}
	}
	return true;
}

bool PeerKey::operator<(const PeerKey& rhs) const {
	for (size_t i = 0; i < data.size(); i++) {
		if (data[i] < rhs.data[i]) {
			return true;
		} else if (data[i] > rhs.data[i]) {
			return false;
		}
	}
	return false; // equal
}

bool PeerKey::operator==(const PeerKey& rhs) const {
	for (size_t i = 0; i < data.size(); i++) {
		if (data[i] != rhs.data[i]) {
			return false;
		}
	}
	return true;
}

} // NGC_EXT

