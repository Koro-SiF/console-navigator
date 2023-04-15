#include <iostream>
#include <map>
#include <string>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include <windows.h>

using namespace std;

// Value-Defintions of the different String values
enum StringValue{ UNDEFINED, 
                    DIR,
                    CD,
                    MKDIR,
                    DEL,
                    OPEN,
                    TOUCH,
                    EXIT };

// struct to save the last write time of files and folders
struct lastwritetime { string dmyTime; string hmsTime;}; 

// Map to associate the strings with the enum values
static map<string, StringValue> mapStringValues;

//function to get the current directory
string get_current_directory() {
    char buf[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, buf);
    return string(buf);
}

//function to get the last write time of a file or folder
lastwritetime get_last_write_time(string file_or_folder_name) {
    string current_dir = get_current_directory();
    WIN32_FIND_DATA file_data;
    lastwritetime lwt;

    HANDLE file_handle = FindFirstFile((current_dir + "\\" + file_or_folder_name).c_str(), &file_data);
    if (file_handle == INVALID_HANDLE_VALUE) {
        lwt.dmyTime = ""; 
        lwt.hmsTime = "";
        return lwt;
    }

    FindClose(file_handle);

    SYSTEMTIME last_write_time;
    FileTimeToSystemTime(&file_data.ftLastWriteTime, &last_write_time);

    lwt.dmyTime = to_string(last_write_time.wDay) + "/" +
                     to_string(last_write_time.wMonth) + "/" +
                     to_string(last_write_time.wYear);
    
    lwt.hmsTime = to_string(last_write_time.wHour) + ":" +
                     to_string(last_write_time.wMinute) + ":" +
                     to_string(last_write_time.wSecond);
    
    return lwt;
}

//function to list the contents of the current directory
vector<string> list_directory_contents() {
    vector<string> contents;
    string current_dir = get_current_directory();
    WIN32_FIND_DATA file_data;

    cout << "Last_Write_Time\t\tName" << endl;
    cout << "---------------\t\t----" << endl;

    HANDLE file_handle = FindFirstFile((current_dir + "\\*").c_str(), &file_data);
    if (file_handle == INVALID_HANDLE_VALUE) {
        return contents;
    }

    do {
        contents.push_back(get_last_write_time(file_data.cFileName).dmyTime + "   " + 
                                                get_last_write_time(file_data.cFileName).hmsTime + "\t" + 
                                                file_data.cFileName);
    } while (FindNextFile(file_handle, &file_data) != 0);
    FindClose(file_handle);

    return contents;
}

//function to change the directory
bool change_directory(string dir) {
    string current_dir = get_current_directory();
    if (SetCurrentDirectory((current_dir + "\\" + dir).c_str())) {
        return true;
    } else if (SetCurrentDirectory(dir.c_str())) {
        return true;
    } else
    return false;
}

//function to create a folder
bool create_folder(string folder_name) {
    string current_dir = get_current_directory();
    if (CreateDirectory((current_dir + "\\" + folder_name).c_str(), NULL)) {
        return true;
    }
    return false;
}

//function to delete a file or folder
bool delete_file_or_folder(string file_or_folder_name) {
    string current_dir = get_current_directory();
    if (DeleteFile((current_dir + "\\" + file_or_folder_name).c_str())) {
        return true;
    }
    return false;
}

//function to open a file or folder
bool open_file_or_folder(string file_or_folder_name) {
    string current_dir = get_current_directory();
    if (ShellExecute(NULL, "open", (current_dir + "\\" + file_or_folder_name).c_str(), NULL, NULL, SW_SHOWNORMAL) > (HINSTANCE) 32) {
        return true;
    }
    return false;
}

//function to create a file
bool create_file(string filename) {
    string current_dir = get_current_directory();
    HANDLE file_handle = CreateFile((current_dir + "\\" + filename).c_str(), 
        GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE) {
        return false;
    }
    CloseHandle(file_handle);
    return true;
}

// User input
static char userInput[_MAX_PATH];

// Intialization
static void Initialize();

int main() {
    string userInput;
    string dir;
    string filename;
    string foldername;
    vector<string> contents;

    Initialize();

    while (1) {
        //get the current directory
        dir = get_current_directory();
        cout << dir << "> ";
        cin >> userInput;

        switch (mapStringValues[userInput]) {
            //list the contents of the directory
            case DIR:
                contents = list_directory_contents();
                for (int i = 0; i < contents.size(); i++) {
                    cout << contents[i] << endl;
                }
                break;

            //change the directory
            case CD:
                cin >> filename;
                if (change_directory(filename)) {
                    cout << "Directory changed successfully.\n";
                } else {
                    cout << "Error changing directory.\n";
                }
                break;

            //create a folder
            case MKDIR:
                cin >> foldername;
                if (create_folder(foldername)) {
                    cout << "Folder created successfully.\n";
                } else {
                    cout << "Error creating folder.\n";
                }
                break;

            //delete a file or folder
            case DEL:
                cin >> filename;
                if (delete_file_or_folder(filename)) {
                    cout << "File/folder deleted successfully.\n";
                } else {
                    cout << "Error deleting file/folder.\n";
                }
                break;

            //open a file or folder
            case OPEN:
                cin >> filename;
                if (open_file_or_folder(filename)) {
                    cout << "File/folder opened successfully.\n";
                } else {
                    cout << "Error opening file/folder.\n";
                }
                break;
            
            //create a file:
            case TOUCH:
                cin >> filename;
                if (create_file(filename)) {
                    cout << "File created successfully.\n";
                } else {
                    cout << "Error creating file";
                }
                break;
            
            //if user inputs unknown command
            case UNDEFINED:
                cout << userInput + ": The term " + userInput + " is not recognised as a name of cmdlet, function, script file, or executable program." << endl << "Check the spelling of the name, or if a path was included, verify that the path is correct and try again.\n";
                break;
            
            //exit the program
            case EXIT:
                return 0;
            
        }

    }
    
}

void Initialize() {
        mapStringValues[""] = UNDEFINED;
        mapStringValues["dir"] = DIR;
        mapStringValues["cd"] = CD;
        mapStringValues["mkdir"] = MKDIR;
        mapStringValues["del"] = DEL;
        mapStringValues["open"] = OPEN;
        mapStringValues["touch"] = TOUCH;
        mapStringValues["exit"] = EXIT;

        // cout << "mapStringValues contains "
        //     << mapStringValues.size() - 1
        //     << " entries." << endl;
    }
  