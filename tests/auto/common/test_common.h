#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <common.h>
#include <stdbool.h>
#include <domain.h>
#include <talloc.h>
#include <verto.h>

#define test_status(x) \
    ld_info("Performing test: %s, status: %s.\n", x.name, x.desired_test_result == rc ? "PASSED" : "FAILED");

char *get_environment_variable(TALLOC_CTX *talloc_ctx, const char *envvar);

int get_current_directory_type(const char* directory_type);

void start_test(verto_callback *update_callback, const int update_interval, int* current_directory_type, bool use_tls);

LDAPAttribute_t** fill_user_attributes(TALLOC_CTX* ctx, LDAPAttribute_t* attrs, int size);

#endif//TEST_COMMON_H
