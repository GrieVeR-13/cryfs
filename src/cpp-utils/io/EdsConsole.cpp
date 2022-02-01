#include "EdsConsole.h"

using std::string;
using std::vector;
using std::shared_ptr;

namespace cpputils {

EdsConsole::EdsConsole() {
}

bool EdsConsole::askYesNo(const string &/*question*/, bool defaultValue) {
    return defaultValue;
}

void EdsConsole::print(const std::string &output) {
//    _baseConsole->print(output);
}

unsigned int EdsConsole::ask(const string &/*question*/, const vector<string> &/*options*/) {
    throw std::logic_error("Tried to ask a multiple choice question in noninteractive mode");
}

string EdsConsole::askPassword(const string &question) {
    return "1"; //todoe
}

}
