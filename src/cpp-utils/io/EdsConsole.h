#pragma once
#ifndef CPPUTILS_IO_EDSCONSOLE_H
#define CPPUTILS_IO_EDSCONSOLE_H

#include "Console.h"

namespace cpputils {

    class EdsConsole final: public Console {
    public:
        EdsConsole(const std::string &password);

        unsigned int ask(const std::string &question, const std::vector<std::string> &options) override;
        bool askYesNo(const std::string &question, bool defaultValue) override;
        void print(const std::string &output) override;
        std::string askPassword(const std::string &question) override;

    private:
        const std::string &password;
    };

}
#endif
