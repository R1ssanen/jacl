#ifndef _TOKENIZE_H_
#define _TOKENIZE_H_

#include "error.h"
#include "tokens.h"
#include "types.h"

enum j_error_t jTokenize(const char* src, u64 src_len, jToken* tokens, u64* token_count);

#endif
