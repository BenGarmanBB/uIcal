/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/logging.h"
#include "uICAL/vline.h"

namespace uICAL {
    VLine::VLine(const string& line) {
        if(line.empty()) {
            this->name = string::none();
            this->value = string::none();
            // throw ParseError("VLINE is empty");
        }

        size_t colon = line.find(":");
        size_t semicolon = line.find(";");

        if (colon == string::npos) {
            this->name = string::none();
            this->value = string::none();
            // throw ParseError(string("VLINE does not have a ':' \"") + line + "\"");
        }

        if (semicolon != string::npos && semicolon < colon) {
            this->name = line.substr(0, semicolon);
            this->readParams(line.substr(semicolon + 1, colon - semicolon - 1));
        }
        else {
            this->name = line.substr(0, colon);
        }
        this->value = line.substr(colon + 1, line.length() - colon - 1);
    }

    string VLine::getParam(const string& key) {
        for (auto param : this->params) {
            if (param.first == key) {
                return param.second;
            }
        }
        return string("");
    }

    void VLine::readParams(const string& str) {
        str.tokenize(';', [&](const string token){
            size_t equals = token.find("=");
            const string name = token.substr(0, equals);
            const string value = token.substr(equals + 1, token.length());
            this->params.insert(std::pair<string, string>(name, value));
        });
    }

    void VLine::str(ostream& out) const {
        out << this->name;
        if (this->params.size()) {
            for (auto kv : this->params) {
                out << ";";
                out << kv.first << "=" << kv.second ;
            }
        }
        out << ":" << this->value;
    }
}
