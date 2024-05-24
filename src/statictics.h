#pragma once

#include "log.h"
#include "time.h"
#include <cstdint>
#include <string>

class Statictics {
  public:
    Statictics(std::string &tag)
        : _tag(tag), _start(rdtsc()){

                     };

    ~Statictics() {
        uint64_t elapse;

        _end = rdtsc();

        elapse = _start - _end;

        slog_info(_tag << ": " << elapse);
    }

  private:
    std::string _tag;
    uint64_t _start;
    uint64_t _end;
};
