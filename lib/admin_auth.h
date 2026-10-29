#ifndef ADMIN_AUTH_H
#define ADMIN_AUTH_H

#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD_SIZE 64

enum admin_login_result
{
    ADMIN_LOGIN_ERROR = -1,
    ADMIN_LOGIN_INVALID = 0,
    ADMIN_LOGIN_SUCCESS = 1
};

/* Create admin.dat with the initial account if it does not exist. */
int admin_initialize(const char *path);

/* Authenticate the reserved admin account and handle password changes. */
enum admin_login_result admin_login(const char *path,
                                    const char *username,
                                    const char *password);

/* Use this in registration to reserve the admin username. */
int admin_is_reserved_username(const char *username);

#endif