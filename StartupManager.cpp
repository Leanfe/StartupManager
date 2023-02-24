//
// Created by lkapi on 24.02.2023.
//

#include "StartupManager.h"

namespace App {

    void StartupManager::add_to_startup() {
        #if defined(_WIN32)
                TCHAR app_path[MAX_PATH];
                GetModuleFileName(nullptr, app_path, MAX_PATH);

                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                std::wstring app_path_wstr = converter.from_bytes(app_path);

                std::wstring startup_folder_path = get_startup_folder_path();
                std::wstring app_link_path = startup_folder_path + L"\\" + str_to_wstr(app_name_) + L".lnk";

                CoInitialize(nullptr);
                IShellLink* link;
                HRESULT result = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&link);
                if (SUCCEEDED(result)) {
                    link->SetPath(reinterpret_cast<LPCSTR>(app_path_wstr.c_str()));
                    link->SetWorkingDirectory(get_app_folder_path().c_str());

                    IPersistFile* file;
                    result = link->QueryInterface(IID_IPersistFile, (LPVOID*)&file);
                    if (SUCCEEDED(result)) {
                        file->Save(app_link_path.c_str(), TRUE);
                        file->Release();
                    }
                    link->Release();
                }
                CoUninitialize();

        #elif defined(__linux__)
                std::string app_path = get_app_folder_path() + "/" + app_name_;
                        std::string startup_script_path = get_startup_script_path();
                        std::string startup_script_content = get_startup_script_content(app_path);

                        std::ofstream startup_script_file(startup_script_path);
                        if (startup_script_file.is_open()) {
                            startup_script_file << startup_script_content;
                            startup_script_file.close();

                            std::filesystem::permissions(startup_script_path, std::filesystem::perms::owner_all | std::filesystem::perms::group_read | std::filesystem::perms::others_read);
                        } else {
                            std::cerr << "Failed to create startup script file: " << strerror(errno) << std::endl;
                            return;
                        }

                        std::filesystem::permissions(startup_script_path, std::filesystem::perms::owner_all | std::filesystem::perms::group_read | std::filesystem::perms::others_read | std::filesystem::perms::owner_exec);

        #endif
    }

    std::string StartupManager::get_app_folder_path() {
        #if defined(_WIN32)
                TCHAR module_path[MAX_PATH];
                GetModuleFileName(nullptr, module_path, MAX_PATH);

                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                std::wstring module_path_wstr = converter.from_bytes(module_path);

                std::string module_path_str(module_path_wstr.begin(), module_path_wstr.end());

                return std::filesystem::path(module_path_str).parent_path().string();

        #elif defined(__linux__)
                char buffer[PATH_MAX];
                        ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
                        if (len != -1) {
                            buffer[len] = '\0';
                            std::string app_path(buffer);
                            return std::filesystem::path(app_path).parent_path().string();
                        } else {
                            std::cerr << "Failed to get app folder path: " << strerror(errno) << std::endl;
                            return "";
                        }
        #endif
    }
} // App