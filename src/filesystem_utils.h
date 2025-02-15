#ifndef FILESYSTEM_UTILS_H
#define FILESYSTEM_UTILS_H

#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>

#if _WIN32
#include <windows.h>
#include "win32dirent.h"
#else // _WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#endif // _WIN32

#if _WIN32
typedef std::wstring path_t;
#define PATHSTR(X) L##X
#else
typedef std::string path_t;
#define PATHSTR(X) X
#endif

bool is_image_file(const std::string& filename)
{
    const std::vector<std::string> extensions = {".jpg", ".jpeg", ".png", ".bmp", ".webp"};

    // Check if the filename contains a dot
    size_t last_dot = filename.find_last_of(".");
    if (last_dot == std::string::npos)
        return false; // No extension

    // Check if it's a hidden file without an extension
    if (filename[0] == '.' && last_dot == 0)
        return false; // No extension

    // Extract the extension and check if it's an image file extension
    std::string extension = filename.substr(last_dot);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return std::find(extensions.begin(), extensions.end(), extension) != extensions.end();
}

#if _WIN32
static bool path_is_directory(const path_t& path)
{
    DWORD attr = GetFileAttributesW(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

static int list_directory(const path_t& dirpath, std::vector<path_t>& imagepaths)
{
    imagepaths.clear();

    _WDIR* dir = _wopendir(dirpath.c_str());
    if (!dir)
    {
        fwprintf(stderr, L"opendir failed %ls\n", dirpath.c_str());
        return -1;
    }

    struct _wdirent* ent = 0;
    while ((ent = _wreaddir(dir)))
    {
        if (ent->d_type != DT_REG)
            continue;

        // Convert wide string to regular string
        std::wstring wfilename(ent->d_name);
        std::string filename(wfilename.begin(), wfilename.end());

        fprintf(stderr, "filename: %s\n", filename.c_str());

        // Check if the file is an image
        if (is_image_file(filename)) {
            fprintf(stderr, "good filename: %s\n", filename.c_str());
            imagepaths.push_back(path_t(wfilename));
        }
    }

    _wclosedir(dir);
    std::sort(imagepaths.begin(), imagepaths.end());

    return 0;
}
#else // _WIN32
static bool path_is_directory(const path_t& path)
{
    struct stat s;
    if (stat(path.c_str(), &s) != 0)
        return false;
    return S_ISDIR(s.st_mode);
}

static int list_directory(const path_t& dirpath, std::vector<path_t>& imagepaths)
{
    imagepaths.clear();

    DIR* dir = opendir(dirpath.c_str());
    if (!dir)
    {
        fprintf(stderr, "opendir failed %s\n", dirpath.c_str());
        return -1;
    }

    struct dirent* ent = 0;
    while ((ent = readdir(dir)))
    {
        if (ent->d_type != DT_REG)
            continue;

        std::string filename(ent->d_name);

        if (is_image_file(filename)) {
            imagepaths.push_back(path_t(filename));
        }
    }

    closedir(dir);
    std::sort(imagepaths.begin(), imagepaths.end());

    return 0;
}
#endif // _WIN32

static path_t get_file_name_without_extension(const path_t& path)
{
    size_t dot = path.rfind(PATHSTR('.'));
    if (dot == path_t::npos)
        return path;

    return path.substr(0, dot);
}

static path_t get_file_extension(const path_t& path)
{
    size_t dot = path.rfind(PATHSTR('.'));
    if (dot == path_t::npos)
        return path_t();

    return path.substr(dot + 1);
}

#if _WIN32
static path_t get_executable_directory()
{
    wchar_t filepath[256];
    GetModuleFileNameW(NULL, filepath, 256);

    wchar_t* backslash = wcsrchr(filepath, L'\\');
    backslash[1] = L'\0';

    return path_t(filepath);
}
#elif ! __linux__ // _WIN32
#include <libproc.h>
static path_t get_executable_directory()
{
    char filepath[256];
    filepath[0]='\0';

    int pid = getpid();
    int ret = proc_pidpath(pid, filepath, sizeof(filepath));

    if ( ret <= 0 ) {
        fprintf(stderr, "PID %d: proc_pidpath ();\n", pid);
        fprintf(stderr, "    %s\n", strerror(errno));
    } else {
        char* slash = strrchr(filepath, '/');
        slash[1] = '\0';
    }

    return path_t(filepath);
}
#else // ! __linux__
static path_t get_executable_directory()
{
    char filepath[256];
    readlink("/proc/self/exe", filepath, 256);

    char* slash = strrchr(filepath, '/');
    slash[1] = '\0';

    return path_t(filepath);
}
#endif // ! __linux__

static bool filepath_is_readable(const path_t& path)
{
#if _WIN32
    FILE* fp = _wfopen(path.c_str(), L"rb");
#else // _WIN32
    FILE* fp = fopen(path.c_str(), "rb");
#endif // _WIN32
    if (!fp)
        return false;

    fclose(fp);
    return true;
}

static path_t sanitize_filepath(const path_t& path)
{
    if (filepath_is_readable(path))
        return path;

    return get_executable_directory() + path;
}

#endif // FILESYSTEM_UTILS_H
