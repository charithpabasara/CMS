# CMS

## Build

From the project root using the MSYS2 UCRT64 toolchain:

```sh
gcc -std=c17 -Wall -Wextra -Wpedantic -Ilib \
    src/main.c src/reg.c src/login.c src/admin_auth.c src/admin_portal.c \
    src/clearscreen.c src/buyfoods.c src/feedback.c src/showprimary.c \
    src/studentoption.c -o cms.exe
```

Run the program from the project root so it can find the `data` directory.
`src/home.c` is a separate legacy entry point and is intentionally not part of
this build because the project uses `src/main.c` as its sole `main()` function.
The admin credentials are initialized in `data/admin.dat`; regular user
accounts are stored in `data/credentials.csv`. Set `ADMIN_FILE` or
`CREDENTIALS_FILE` to override those file paths.
