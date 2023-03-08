#pragma once
#include <cstring>
#include <dirent.h>
#include <vector>
#include <iostream>
using namespace std;
 
inline void GetFileNames(string path,vector<string>& filenames)
{
    DIR *pDir;
    struct dirent* ptr;
    vector<string> local_filenames;
    if(!(pDir = opendir(path.c_str()))){
        cout<<"Folder doesn't Exist!"<<endl;
        return;
    }
    while((ptr = readdir(pDir))!=0) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0){
            local_filenames.push_back(path + "/" + ptr->d_name);
            if(local_filenames.back().find(".jpg") == string::npos && local_filenames.back().find(".png") == string::npos)
                continue;
            filenames.push_back(path + "/" + ptr->d_name);
        }
    }
    closedir(pDir);
}