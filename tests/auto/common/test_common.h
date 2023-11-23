#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <talloc.h>

char *get_environment_variable(TALLOC_CTX *talloc_ctx, const char *envvar);

int get_current_directory_type(const char* directory_type);

#endif//TEST_COMMON_H
