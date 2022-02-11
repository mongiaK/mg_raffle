/*================================================================
*
*  文件名称：main.cpp
*  创 建 者: mongia
*  创建日期：2022年01月26日
*
================================================================*/
#include <bits/stdc++.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/mman.h>

#include <cstdio>
#include <ios>
#include <istream>
#include <random>

char* cmd_long_options_to_short_options(const struct option options[]) {
    char short_options[UCHAR_MAX * 3 + 1];
    char* p = short_options;

    for (; options->name; options++) {
        const struct option* o = options;
        if (o->flag == NULL && o->val > 0 && o->val <= UCHAR_MAX) {
            *p++ = o->val;
            if (o->has_arg == required_argument) {
                *p++ = ':';
            } else if (o->has_arg == optional_argument) {
                *p++ = ':';
                *p++ = ':';
            }
        }
    }
    *p++ = '\0';

    return strdup(short_options);
}

void parse_options(int argc, char* argv[]) {
    static const struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {NULL, 0, NULL, 0},
    };

    char* short_options = cmd_long_options_to_short_options(long_options);
    for (;;) {
        int c = getopt_long(argc, argv, short_options, long_options, NULL);
        if (-1 == c) {
            break;
        }

        switch (c) {
            case 'h':
                break;
            case 'v':
                break;
            default:
                abort();
        }
    }

    free(short_options);
}

class SpendTime {
   public:
    explicit SpendTime(const char* tag)
        : _tag(tag), _start(std::chrono::steady_clock::now()) {}
    ~SpendTime() {
        _end = std::chrono::steady_clock::now();

        std::cout
            << _tag << ", time cost: "
            << std::chrono::duration<double, std::milli>(_end - _start).count()
            << "ms" << std::endl;
    }

   private:
    std::string _tag;
    std::chrono::steady_clock::time_point _start;
    std::chrono::steady_clock::time_point _end;
};

#define size 10000
std::vector<std::string> data;

std::string random_string() {
    std::string str(
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(str.begin(), str.end(), generator);

    return str.substr(0, 32);
}

void prepare() {
    data.resize(size);

    for (int i = 0; i < size; i++) {
        data[i] = random_string();
    }
}

void ofile() {
    std::ofstream of;
    of.open("ofile.txt");

    std::ios::sync_with_stdio(false);
    std::cout.tie(0);

    SpendTime st("ofile");
    for (auto& v : data) {
        of << v << "\n";
    }
}

void pfile() {
    FILE* fp = fopen("pfile.txt", "ab+");
    if (nullptr == fp) {
        return;
    }

    {
        SpendTime st("pfile");
        for (auto& v : data) {
            fprintf(fp, "%s", v.c_str());
        }
    }
    fclose(fp);
}

void fwfile() {
    FILE* fp = fopen("fwfile.txt", "ab+");
    if (nullptr == fp) {
        return;
    }

    {
        SpendTime st("fwfile");
        for (auto& v : data) {
            fwrite(v.c_str(), v.length(), 1, fp);
        }
    }
    fclose(fp);
}

void wfile() {
    int fp = open("wfile.txt", O_RDWR | O_CREAT, 0666);
    if (fp < 0) {
        return;
    }

    {
        SpendTime st("wfile");
        for(auto& v: data) {
            write(fp, v.c_str(), v.length());
        }
    }
    close(fp);
}

void mmapwfile() {
    int fp = open("mmapwfile.txt", O_RDWR | O_CREAT, 0666);
    if (fp < 0) {
        return;
    }
    int expand = 1024 * 1000;
    int asize = lseek(fp, 0, SEEK_END);
    int total = 0;
    char * addr = nullptr;
    if (asize < 0) {
        goto closefp;
    }    
    
    if (lseek(fp, expand, SEEK_SET) < 0) {
        goto closefp;
    }

    if (asize == 0) {
        write(fp, "", 1);
    }

    addr = (char*)mmap(NULL, expand, PROT_READ | PROT_WRITE,
                             MAP_FILE | MAP_SHARED, fp, 0);
    if (addr == MAP_FAILED) {
        goto closefp;
    }
    {
        SpendTime st("mmapwfile");
        for(auto& v: data) {
            memcpy(addr, v.c_str(), v.length());
            addr += v.length();
            total += v.length();
        }
    }

    munmap(addr, total);
closefp:
    close(fp);
}

int main(int argc, char* argv[]) {
    parse_options(argc, argv);

    prepare();

    ofile();
    
    pfile();
    
    fwfile();

    mmapwfile();

    wfile();

    return 0;
}
