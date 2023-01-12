#ifndef C_NGC_EXT_H
#define C_NGC_EXT_H

// this is a c header

#include <tox/tox.h>

#ifdef __cplusplus
extern "C" {
#endif

// copy from tox.h:
#ifndef TOX_DEFINED
#define TOX_DEFINED
typedef struct Tox Tox;
#endif /* TOX_DEFINED */

typedef struct NGC_EXT_CTX NGC_EXT_CTX;

NGC_EXT_CTX* NGC_EXT_new(void);
void NGC_EXT_kill(NGC_EXT_CTX* ngc_ext_ctx);

void NGC_EXT_handle_group_custom_packet(
	Tox* tox,
	NGC_EXT_CTX* ngc_ext_ctx,

	uint32_t group_number,
	uint32_t peer_number,

	// bool private,

	const uint8_t *data,
	size_t length
	//void *user_data
);

#ifdef __cplusplus
}
#endif

#endif // C_NGC_EXT_H

