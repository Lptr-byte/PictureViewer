#pragma once
#include <cstring>
#include <dirent.h>
#include <vector>
#include <iostream>
using namespace std;
/*
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
}*/

inline void GetFileNames(string path, vector<string> &files)
{
    long hFile = 0;
    struct _finddata_t fileinfo;
    string pathp;
    if ((hFile = _findfirst(pathp.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            if ((fileinfo.attrib &  _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                {					
                    continue;
                }
            }
            else
            {
                string filestr = fileinfo.name;
				if(filestr.find(".jpg") == string::npos && filestr.find(".png") == string::npos)
                    continue;
                files.push_back(path + "/" + filestr);
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}
