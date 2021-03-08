/*================================================================
*  
*  文件名称：music_change_name.cpp
*  创 建 者: mongia
*  创建日期：2020年12月04日
*  描    述：mr_pengmj@outlook.com
*  
================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

static std::string music_dir_path;
static const std::string music_type = ".mp3";

static void usage()
{
    std::cout << "usage: " << std::endl;
    std::cout << "  -h help" << std::endl;
    std::cout << "  -d music path (absolute path)" << std::endl;
}

static void parse_args(int argc, char* argv[])
{
    int opt;
    while(-1 != (opt = getopt(argc, argv, "d:h"))) {
        switch(opt) {
            case 'h':
                usage();
                exit(0);
                break;
            case 'd':
                music_dir_path = optarg;
                break;
            default:
                break;
        }
    }

    if(music_dir_path.length() == 0) {
        std::cerr << "输入的音乐文件夹路径为空" << std::endl;
        exit(1);
    }

    if(access(music_dir_path.c_str(), 0) != 0) {
        std::cerr << "请输入正确的音乐文件夹路径" << std::endl;
        exit(1);
    }

    if(music_dir_path.at(music_dir_path.length() - 1) != '/') {
        music_dir_path += "/";
    }
}

static void get_files(const std::string& dir_path, std::vector<std::string>& files)
{
    DIR* pdir = NULL;
    struct dirent* ptr = NULL;

    struct stat s;
    lstat(dir_path.c_str(), &s);

    if(!S_ISDIR(s.st_mode)) {
        std::cout << "请输入音乐文件夹绝对路径！" << std::endl;
        exit(1);
    }

    if(!(pdir = opendir(dir_path.c_str()))) {
        std::cout << "打开音乐文件夹失败！" << std::endl;
        exit(1);
    }

    while(NULL != (ptr = readdir(pdir))) {
        std::string filename = ptr->d_name;
        if(ptr->d_type != DT_REG || filename.length() < 4 || filename.substr(filename.length() - 4, 4) != music_type) 
            continue;

        filename = dir_path + filename;
        files.push_back(filename);
    }

    closedir(pdir);
}

static std::string id3v1_parse(FILE* fp)
{
    if(fseek(fp, -128, SEEK_END)) {
        std::cout << "irregular mp3 file. not id3v1 or id3v2" << std::endl;
        return "";
    } // ID3v1 音乐信息存储在mp3文件末尾的128个字节中

    char info[128];
    fread(info, 1, 128, fp);

    char* p = info;
    if(strncmp(p, "TAG", 3) != 0) {
        std::cout << "id3v1 not TAG in first 3 bytes" << std::endl;
        return "";
    }

    p += 3;

    std::string name = p;
    name.resize(30); // 这30个字节是歌名

    return name;
}

static std::string get_music_name(const std::string& music)
{
    FILE* fp = NULL;
    char *p = NULL;
    std::string name;

    if((fp = fopen(music.c_str(), "rb")) == NULL) {
        std::cout << music << " open failed, errno: " << errno << std::endl;
        return "";
    }

    // ID3 | version | flags | size
    // 3byte + 2byte + 1byte + 4byte = 10byte
    char head[11] = {0}; 
    int ret = fread(head, 1, 10, fp);
    if (ret < 10) {
        std::cout << music << "irregular mp3 file." << std::endl;
        return "";
    }

    p = head;
    if(strncmp(p, "ID3", 3) != 0) {
        std::cout << music << " mp3 version is not id3v2." << std::endl;
        // maybe id3v1
        name = id3v1_parse(fp);
        fclose(fp);
        return name;
    }

    // ID3V2
    p += 6;
    int info_length = *(reinterpret_cast<int*>(p));

    std::shared_ptr<char> info(new char[info_length]);
    ret = fread(info.get(), 1, info_length, fp);

    fclose(fp);
    return name;
}

static void rename_music_files(const std::vector<std::string>& files)
{
    for(auto &f : files) {
        std::string music_name = get_music_name(f);
        if(music_name.length() != 0) {
            std::string new_name = music_dir_path + music_name + music_type;
            rename(f.c_str(), new_name.c_str());
        }
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::string> music_files;
    
    parse_args(argc, argv);

    get_files(music_dir_path, music_files);

    rename_music_files(music_files);

    return 0;
}
