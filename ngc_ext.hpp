#pragma once

#include <tox/tox.h>

#include "ngc_ext.h"

#include <array>

// public scope

using handle_group_custom_packet_cb = void(*)(
	Tox* tox,
	NGC_EXT_CTX* ngc_ext_ctx, // TODO: useless?

	uint32_t group_number,
	uint32_t peer_number,

	const uint8_t* data,
	size_t length,
	void* user_data
);

struct NGC_EXT_CTX {
	std::array<handle_group_custom_packet_cb, 256> callbacks;
	std::array<void*, 256> user_data;
};

namespace NGC_EXT {

// start with first bit set, to avoid collisions (this will have to change, since those will be standadized?)
enum PacketType : uint8_t {
	// TODO: why?
	INVALID = 0u,

	//TODO: make it possible to go further back
	// request last (few) message_ids for a peer
	// - peer_key bytes (peer key we want to know ids for)
	// - 1 byte (uint8_t count ids, atleast 1)
	HS1_REQUEST_LAST_IDS = 0x80 | 1u,

	// respond to a request with 0 or more message ids, sorted by newest first
	// - peer_key bytes (the msg_ids are from)
	// - 1 byte (uint8_t count ids, can be 0)
	// - array [
	//   - msg_id bytes (the message id)
	// - ]
	HS1_RESPONSE_LAST_IDS,

	// request the other side to initiate a FT
	// - 1 byte (file_kind)
	// - X bytes (file_kind dependent id, differnt sizes)
	FT1_REQUEST = 0x80 | 8u,

	// TODO: request result negative, speed up not found

	// tell the other side you want to start a FT
	// TODO: might use id layer instead. with it, it would look similar to friends_ft
	// - 1 byte (file_kind)
	// - 8 bytes (data size, can be 0 if unknown, BUT files have to be atleast 1 byte)
	// - 1 byte (temporary_file_tf_id, for this peer only, technically just a prefix to distinguish between simultainious fts)
	// - X bytes (file_kind dependent id, differnt sizes)
	FT1_INIT,

	// acknowlage init (like an accept)
	// like tox ft control continue
	// - 1 byte (transfer_id)
	FT1_INIT_ACK,

	// TODO: init deny, speed up non acceptance

	// data fragment
	// - 1 byte (temporary_file_tf_id)
	// - 2 bytes (sequece id)
	// - X bytes (the data fragment)
	// (size is implicit)
	FT1_DATA,

	// acknowlage data fragments
	// TODO: last 3 should be sufficient, 5 should be generous
	// - 1 byte (temporary_file_tf_id)
	// // this is implicit (pkg size)- 1 byte (number of sequence ids to ack, this kind of depends on window size)
	// - array [ (of sequece ids)
	//   - 2 bytes (sequece id)
	// - ]
	FT1_DATA_ACK,
};

// TODO: using alias would be fine
struct GroupKey {
	std::array<uint8_t, TOX_GROUP_CHAT_ID_SIZE> data;

	GroupKey(void) = default;
	GroupKey(const GroupKey& other) : data(other.data) {}
	GroupKey(GroupKey&&) = delete;
	GroupKey& operator=(const GroupKey& other) { data = other.data; return *this; }
	bool operator<(const GroupKey& rhs) const;
	bool operator==(const GroupKey& rhs) const;
	size_t size(void) const { return data.size(); }
};

struct PeerKey {
	std::array<uint8_t, TOX_GROUP_PEER_PUBLIC_KEY_SIZE> data;

	PeerKey(void) = default;
	PeerKey(const PeerKey& other) : data(other.data) {}
	PeerKey(PeerKey&&) = delete;
	PeerKey& operator=(const PeerKey& other) { data = other.data; return *this; }
	bool operator<(const PeerKey& rhs) const;
	bool operator==(const PeerKey& rhs) const;
	size_t size(void) const { return data.size(); }
};

[[maybe_unused]] static const char* pkgid2str(PacketType type) {
#define _EXT_CASE(x) case (x): return #x;
	switch (type) {
		_EXT_CASE(INVALID)

		_EXT_CASE(HS1_REQUEST_LAST_IDS)
		_EXT_CASE(HS1_RESPONSE_LAST_IDS)

		_EXT_CASE(FT1_REQUEST)
		_EXT_CASE(FT1_INIT)
		_EXT_CASE(FT1_INIT_ACK)
		_EXT_CASE(FT1_DATA)
		_EXT_CASE(FT1_DATA_ACK)

		default: return "<unk>";
	}
#undef _EXT_CASE
}

} // NGC_EXT

