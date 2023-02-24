//
// Created by lkapi on 24.02.2023.
//

#ifndef VKSTATUSER_STARTUPMANAGER_H
#define VKSTATUSER_STARTUPMANAGER_H

#include <iostream>
#include <string>
#include <filesystem>
#include <utility>

#if defined(_WIN32)
#include <Windows.h>
#include <ShlObj.h>
#endif

using namespace std;

namespace App {

    class StartupManager {
    public:
        explicit StartupManager(string  app_name) : app_name_(std::move(app_name)) {}

        void add_to_startup();

    private:
        string app_name_;

        static string get_app_folder_path();

        #if defined(_WIN32)
            static wstring get_startup_folder_path() {
                wchar_t* app_data_path;
                SHGetKnownFolderPath(FOLDERID_Startup, 0, nullptr, &app_data_path);
                wstring startup_folder_path(app_data_path);
                CoTaskMemFree(app_data_path);
                return startup_folder_path;
            }

            static wstring str_to_wstr(const string& str) {
                wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
                return converter.from_bytes(str);
            }

        #elif defined(__linux__)
            string get_startup_script_path() {
                    string home_path = getenv("HOME");
                    return home_path + "/.config/autostart/" + app_name_ + ".desktop";
            }

            string get_startup_script_content(const string& app_path) {
                return "[Desktop Entry]\n"
                       "Type=Application\n"
                       "Exec=" + app_path + "\n"
                       "Hidden=false\n"
                       "NoDisplay=false\n"
                       "X-GNOME-Autostart-enabled=true\n"
                       "Name=" + app_name_ + "\n"
                       "Comment=" + app_name_ + " autostart\n";
            }
        #endif
    };

} // App

#endif //VKSTATUSER_STARTUPMANAGER_H
