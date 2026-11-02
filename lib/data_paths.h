#ifndef CMS_DATA_PATHS_H
#define CMS_DATA_PATHS_H

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define CMS_GETCWD _getcwd
#else
#include <unistd.h>
#define CMS_GETCWD getcwd
#endif

static const char *cmsResolveDataDirectory(void)
{
    static char data_directory[4096];
    static const char *const menu_files[] = {
        "meals.csv", "snacks.csv", "beverages.csv", "desserts.csv"
    };
    char current_directory[4096];

    if (CMS_GETCWD(current_directory, sizeof current_directory) == NULL) {
        return NULL;
    }

    for (;;) {
        int length = snprintf(data_directory, sizeof data_directory,
                              "%s/data", current_directory);
        if (length >= 0 && (size_t)length < sizeof data_directory) {
            int all_menus_found = 1;
            for (size_t i = 0; i < sizeof menu_files / sizeof menu_files[0]; ++i) {
                char menu_path[4096];
                length = snprintf(menu_path, sizeof menu_path,
                                  "%s/menu/%s", data_directory, menu_files[i]);
                if (length < 0 || (size_t)length >= sizeof menu_path) {
                    all_menus_found = 0;
                    break;
                }

                FILE *menu = fopen(menu_path, "r");
                if (menu == NULL) {
                    all_menus_found = 0;
                    break;
                }
                fclose(menu);
            }
            if (all_menus_found) {
                return data_directory;
            }
        }

        char *separator = strrchr(current_directory, '/');
        char *backslash = strrchr(current_directory, '\\');
        if (backslash != NULL &&
            (separator == NULL || backslash > separator)) {
            separator = backslash;
        }
        if (separator == NULL) {
            return NULL;
        }
#ifdef _WIN32
        if (separator == current_directory + 2 && current_directory[1] == ':') {
            separator[1] = '\0';
        } else
#endif
        if (separator == current_directory) {
            separator[1] = '\0';
        } else {
            *separator = '\0';
        }
    }
}

static int cmsBuildDataPath(const char *relative_path, char *path,
                            size_t path_size)
{
    const char *data_directory = cmsResolveDataDirectory();
    int length;

    if (data_directory == NULL) {
        return 0;
    }

    length = snprintf(path, path_size, "%s/%s", data_directory, relative_path);
    return length >= 0 && (size_t)length < path_size;
}

#endif
