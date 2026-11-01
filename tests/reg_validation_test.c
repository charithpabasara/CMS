#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

bool validate_username(const char *username);
bool validate_nic(const char *nic);

int admin_is_reserved_username(const char *username)
{
    (void)username;
    return 0;
}

int main(void)
{
    assert(!validate_username(""));
    assert(!validate_username("a"));
    assert(!validate_username("abcdef"));
    assert(validate_username("abcdefg"));
    assert(validate_username("user_007"));
    assert(!validate_username("user name"));

    assert(!validate_nic(""));
    assert(!validate_nic("123456789012"));
    assert(validate_nic("1234567890123"));
    assert(!validate_nic("12345678901234"));
    assert(!validate_nic("123456789012a"));

    puts("Registration validation tests passed.");
    return 0;
}
