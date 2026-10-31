#include "../lib/html2pdf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <process.h>
#include <direct.h>
#include <windows.h>

#define MKDIR(p) _mkdir(p)
#define FILE_EXISTS(p) (_access((p), 0) == 0)
#define IS_EXECUTABLE(p) FILE_EXISTS(p)
#define SYSTEM(cmd) system(cmd)
#else
#include <unistd.h>

#define MKDIR(p) mkdir((p), 0755)
#define FILE_EXISTS(p) (access((p), F_OK) == 0)
#define IS_EXECUTABLE(p) (access((p), X_OK) == 0)
#define SYSTEM(cmd) system(cmd)
#endif

#define CHS_VERSION "156.0.8078.73"
#define CHS_BASE_URL \
    "https://storage.googleapis.com/chrome-for-testing-public/" \
    CHS_VERSION "/%s/chrome-headless-shell-%s.zip"
#define CHS_INSTALL_DIR "../data/headlessui/.cache/chrome-headless-shell/" CHS_VERSION
#define BROWSER_CONFIG "../data/headlessui/.cache/html2pdf-browser.txt"

static void enable_ansi_colors(void) {
#ifdef _WIN32
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;

    SetConsoleOutputCP(CP_UTF8);

    if (console != INVALID_HANDLE_VALUE &&
        GetConsoleMode(console, &mode)) {
        SetConsoleMode(console, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif
}

static void show_welcome_file(void) {
    FILE *file = fopen("../data/headlessui/welcome.ans", "rb");

    /* Continue with PDF creation if the optional banner file is missing. */
    if (!file) {
        fprintf(stderr, "[html2pdf] WARNING: could not open welcome.ans\n");
        return;
    }

    enable_ansi_colors();

#ifdef _WIN32
    SYSTEM("cls");
#else
    SYSTEM("clear");
#endif

    char buffer[4096];
    size_t count;

    while ((count = fread(buffer, 1, sizeof buffer, file)) > 0) {
        fwrite(buffer, 1, count, stdout);
    }

    printf("\n\n\n\n\n");

    fclose(file);
    fflush(stdout);
}

static int is_regular_file(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static int is_directory(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static int make_cache_dirs(void) {
    const char *dirs[] = {
        "../data/headlessui/.cache",
        "../data/headlessui/.cache/chrome-headless-shell",
        CHS_INSTALL_DIR
    };

    for (size_t i = 0; i < sizeof dirs / sizeof dirs[0]; i++) {
        if (!is_directory(dirs[i]) &&
            MKDIR(dirs[i]) != 0 &&
            !is_directory(dirs[i])) {
            fprintf(stderr, "[html2pdf] ERROR: could not create directory: %s\n",
                    dirs[i]);
            return -1;
        }
    }

    return 0;
}

/* Save an explicitly selected browser path for future runs. */
int set_browser_path(const char *path) {
    if (!path || !is_regular_file(path) || !IS_EXECUTABLE(path)) {
        fprintf(stderr, "[html2pdf] ERROR: invalid browser path\n");
        return -1;
    }

    if (!is_directory("../data/headlessui/.cache") &&
    MKDIR("../data/headlessui/.cache") != 0 &&
    !is_directory("../data/headlessui/.cache")) {
    fprintf(stderr, "[html2pdf] ERROR: could not create .cache directory\n");
    return -1;
    }

    FILE *f = fopen(BROWSER_CONFIG, "w");
    if (!f) {
        fprintf(stderr, "[html2pdf] ERROR: could not save browser path\n");
        return -1;
    }

    fprintf(f, "%s\n", path);
    if (fclose(f) != 0) {
        fprintf(stderr, "[html2pdf] ERROR: could not save browser path\n");
        return -1;
    }

    return 0;
}

static int get_saved_browser(char *out, size_t out_len) {
    char path[2048];
    FILE *f = fopen(BROWSER_CONFIG, "r");

    if (!f)
        return -1;

    if (!fgets(path, sizeof path, f)) {
        fclose(f);
        return -1;
    }
    fclose(f);

    path[strcspn(path, "\r\n")] = '\0';

    if (!is_regular_file(path) || !IS_EXECUTABLE(path))
        return -1;

    snprintf(out, out_len, "%s", path);
    return 0;
}

static const char *get_platform(void) {
#ifdef _WIN32
    return "win64";
#elif defined(__APPLE__)
    #if defined(__aarch64__)
        return "mac-arm64";
    #else
        return "mac-x64";
    #endif
#else
    #if defined(__aarch64__)
        return "linux-arm64";
    #else
        return "linux64";
    #endif
#endif
}

static int find_system_browser(char *out, size_t out_len) {
#ifdef _WIN32
    const char *pf = getenv("ProgramFiles");
    const char *pf86 = getenv("ProgramFiles(x86)");
    char chrome[1024];
    char edge[1024];

    if (!pf)
        pf = "C:\\Program Files";
    if (!pf86)
        pf86 = "C:\\Program Files (x86)";

    snprintf(chrome, sizeof chrome,
             "%s\\Google\\Chrome\\Application\\chrome.exe", pf);
    snprintf(edge, sizeof edge,
             "%s\\Microsoft\\Edge\\Application\\msedge.exe", pf86);

    const char *candidates[] = { chrome, edge };
#elif defined(__APPLE__)
    const char *candidates[] = {
        "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome",
        "/Applications/Microsoft Edge.app/Contents/MacOS/Microsoft Edge",
        "/Applications/Chromium.app/Contents/MacOS/Chromium"
    };
#else
    const char *candidates[] = {
        "/usr/bin/google-chrome",
        "/usr/bin/google-chrome-stable",
        "/usr/bin/microsoft-edge",
        "/usr/bin/chromium"
    };
#endif

    for (size_t i = 0; i < sizeof candidates / sizeof candidates[0]; i++) {
        if (is_regular_file(candidates[i]) && IS_EXECUTABLE(candidates[i])) {
            snprintf(out, out_len, "%s", candidates[i]);
            return 0;
        }
    }

    return -1;
}

static int download_and_install(char *out, size_t out_len) {
    const char *platform = get_platform();
    char url[512];
    char zip_path[512];
    char extract_dir[512];
    char command[2048];

    if (make_cache_dirs() != 0)
        return -1;

    snprintf(zip_path, sizeof zip_path, "%s/chs-%s.zip",
             CHS_INSTALL_DIR, platform);
    snprintf(extract_dir, sizeof extract_dir,
             "%s/chrome-headless-shell-%s", CHS_INSTALL_DIR, platform);
    snprintf(url, sizeof url, CHS_BASE_URL, platform, platform);

    printf("[html2pdf] Downloading browser (%s)...\n", platform);

#ifdef _WIN32
    snprintf(command, sizeof command,
             "curl.exe -fL -o \"%s\" \"%s\"", zip_path, url);
#else
    snprintf(command, sizeof command,
             "curl -fL -o '%s' '%s'", zip_path, url);
#endif

    if (SYSTEM(command) != 0) {
        fprintf(stderr, "[html2pdf] ERROR: download failed\n");
        return -1;
    }

    printf("[html2pdf] Extracting browser...\n");

#ifdef _WIN32
    snprintf(command, sizeof command,
             "tar -xf \"%s\" -C \"%s\"", zip_path, CHS_INSTALL_DIR);
#else
    snprintf(command, sizeof command,
             "tar -xf '%s' -C '%s'", zip_path, CHS_INSTALL_DIR);
#endif

    if (SYSTEM(command) != 0) {
        fprintf(stderr, "[html2pdf] ERROR: extraction failed\n");
        return -1;
    }

    remove(zip_path);

#ifdef _WIN32
    snprintf(out, out_len, "%s\\chrome-headless-shell.exe", extract_dir);
#else
    snprintf(out, out_len, "%s/chrome-headless-shell", extract_dir);
#endif

    if (!is_regular_file(out) || !IS_EXECUTABLE(out)) {
        fprintf(stderr, "[html2pdf] ERROR: browser binary not found: %s\n", out);
        return -1;
    }

    return 0;
}

static int prompt_for_browser(char *out, size_t out_len) {
    char path[2048];

    printf("Browser not found automatically.\n"
           "Enter the full path to Chrome, Edge, Chromium, or "
           "chrome-headless-shell.\n"
           "Press Enter without a path to download chrome-headless-shell: ");
    fflush(stdout);

    if (!fgets(path, sizeof path, stdin))
        return download_and_install(out, out_len);

    path[strcspn(path, "\r\n")] = '\0';

    if (path[0] == '\0')
        return download_and_install(out, out_len);

    if (set_browser_path(path) != 0)
        return -1;

    snprintf(out, out_len, "%s", path);
    return 0;
}

static int get_browser_path(char *out, size_t out_len) {
    char cached[1024];

    /* Use a valid saved choice first. */
    if (get_saved_browser(out, out_len) == 0)
        return 0;

    /* Then check for a previously downloaded headless shell. */
    snprintf(cached, sizeof cached, "%s/chrome-headless-shell-%s",
             CHS_INSTALL_DIR, get_platform());
#ifdef _WIN32
    strncat(cached, "\\chrome-headless-shell.exe",
            sizeof cached - strlen(cached) - 1);
#else
    strncat(cached, "/chrome-headless-shell",
            sizeof cached - strlen(cached) - 1);
#endif

    if (is_regular_file(cached) && IS_EXECUTABLE(cached)) {
        snprintf(out, out_len, "%s", cached);
        return 0;
    }

    /* Then scan standard browser locations. */
    if (find_system_browser(out, out_len) == 0) {
        set_browser_path(out);
        return 0;
    }

    return prompt_for_browser(out, out_len);
}

static void make_file_url(const char *path, char *out, size_t out_len) {
#ifdef _WIN32
    char absolute[2048];

    if (!_fullpath(absolute, path, sizeof absolute))
        snprintf(absolute, sizeof absolute, "%s", path);

    size_t j = 0;
    const char prefix[] = "file:///";

    for (size_t i = 0; prefix[i] && j + 1 < out_len; i++)
        out[j++] = prefix[i];

    for (size_t i = 0; absolute[i] && j + 1 < out_len; i++) {
        char c = absolute[i] == '\\' ? '/' : absolute[i];

        if (c == ' ' && j + 3 < out_len) {
            out[j++] = '%';
            out[j++] = '2';
            out[j++] = '0';
        } else {
            out[j++] = c;
        }
    }

    out[j] = '\0';
#else
    char absolute[2048];

    if (!realpath(path, absolute))
        snprintf(absolute, sizeof absolute, "%s", path);

    snprintf(out, out_len, "file://%s", absolute);
#endif
}

static int make_absolute_path(const char *path, char *out, size_t out_len) {
#ifdef _WIN32
    return _fullpath(out, path, out_len) ? 0 : -1;
#else
    if (path[0] == '/') {
        int n = snprintf(out, out_len, "%s", path);
        return n >= 0 && (size_t)n < out_len ? 0 : -1;
    }

    char cwd[2048];
    if (!getcwd(cwd, sizeof cwd))
        return -1;

    int n = snprintf(out, out_len, "%s/%s", cwd, path);
    return n >= 0 && (size_t)n < out_len ? 0 : -1;
#endif
}

int html_to_pdf(const char *html_path, const char *pdf_path) {
    static int welcome_shown = 0;

    if (!welcome_shown) {
        show_welcome_file();
        welcome_shown = 1;
    }

    char browser[2048];
    char file_url[4096];

    if (!html_path || !pdf_path) {
        fprintf(stderr, "[html2pdf] ERROR: input and output paths are required\n");
        return -1;
    }

    char absolute_pdf[4096];
    char pdf_arg[4096];

    if (make_absolute_path(pdf_path, absolute_pdf, sizeof absolute_pdf) != 0) {
        fprintf(stderr, "[html2pdf] ERROR: could not resolve PDF output path\n");
        return -1;
    }

    snprintf(pdf_arg, sizeof pdf_arg, "--print-to-pdf=%s", absolute_pdf);


    if (get_browser_path(browser, sizeof browser) != 0) {
        fprintf(stderr, "[html2pdf] ERROR: could not obtain a browser binary\n");
        return -1;
    }

    make_file_url(html_path, file_url, sizeof file_url);

    printf("Creating PDF: %s\n", pdf_path);
    fflush(stdout);

#ifdef _WIN32
    const char *args[] = {
        browser,
        "--headless",
        "--no-pdf-header-footer",
        "--no-sandbox",
        "--disable-gpu",
        "--disable-logging",
        "--log-level=3",
        "--virtual-time-budget=5000",
        pdf_arg,
        file_url,
        NULL
    };

    fflush(stdout);
    fflush(stderr);

    int saved_stdout = _dup(_fileno(stdout));
    int saved_stderr = _dup(_fileno(stderr));
    int null_fd = _open("NUL", _O_WRONLY);

    if (null_fd != -1) {
        _dup2(null_fd, _fileno(stdout));
        _dup2(null_fd, _fileno(stderr));
        _close(null_fd);
    }

    int result = _spawnv(_P_WAIT, browser, args);

    if (saved_stdout != -1) {
        _dup2(saved_stdout, _fileno(stdout));
        _close(saved_stdout);
    }

    if (saved_stderr != -1) {
        _dup2(saved_stderr, _fileno(stderr));
        _close(saved_stderr);
    }
#else
    char command[8192];

    snprintf(command, sizeof command,
             "\"%s\" --headless --no-pdf-header-footer "
             "--no-sandbox --disable-gpu --disable-logging "
             "--log-level=3 --virtual-time-budget=5000 "
             "\"%s\" \"%s\" >/dev/null 2>&1",
             browser, pdf_arg, file_url);

    int result = SYSTEM(command);
#endif

    if (result != 0) {
        fprintf(stderr, "[html2pdf] ERROR: browser command failed\n");
        return -1;
    }

   FILE *pdf = fopen(absolute_pdf, "rb");
    if (!pdf) {
        fprintf(stderr, "[html2pdf] ERROR: no PDF produced\n");
        return -1;
    }

    if (fseek(pdf, 0, SEEK_END) != 0) {
        fclose(pdf);
        fprintf(stderr, "[html2pdf] ERROR: could not check PDF size\n");
        return -1;
    }

    long size = ftell(pdf);
    fclose(pdf);

    if (size <= 0) {
        fprintf(stderr, "[html2pdf] ERROR: PDF is empty\n");
        return -1;
    }

    printf("Done.\n");
    return 0;
}
