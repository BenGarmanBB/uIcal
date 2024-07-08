/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/tzmap.h"
#include "uICAL/tz.h"
#include "uICAL/vline.h"
#include "uICAL/vobject.h"
#include <UrlEncode.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

namespace uICAL {
    TZMap::TZMap() {
    }

    void TZMap::add(const VObject_ptr& timezone) {
        Serial.println("Adding timezone");
        string tzId = timezone->getPropertyByName("TZID")->value;
        const char* baseUrl = "https://partflow-test.blueberrytest.com/api/device/getTimezoneOffset";

        time_t now = time(nullptr);
        struct tm timeinfo;
        gmtime_r(&now, &timeinfo);
        char dateStr[11];
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &timeinfo);

        StaticJsonDocument<200> jsonBody;
        jsonBody["timezone"] = tzId.c_str();
        jsonBody["date"] = dateStr;
        
        char buffer[200];
        size_t n = serializeJson(jsonBody, buffer);
        buffer[n] = '\0';
        HTTPClient http;
        http.begin(baseUrl);
        http.addHeader("Content-Type", "application/json");
        int httpCode = http.POST(buffer);
        if (httpCode > 0) {
            String payload = http.getString();
            payload.replace("\"", "");
            int offset = payload.toInt();
            this->add(tzId, tzId, offset);
        }
    }

    void TZMap::add(const string& id, const string& name, int offset) {
        this->id_attrib_map[id].offset = offset;
        this->id_attrib_map[id].name = name;
    }

    void TZMap::add(const string& id, const string& name, const string& tz) {
        this->id_attrib_map[id].offset = TZ::parseOffset(tz);
        this->id_attrib_map[id].name = name;
    }

    string TZMap::findId(const string& nameOrId) const {
        for (auto i : this->id_attrib_map) {
            if (i.second.name == nameOrId || i.first == nameOrId) {
                return i.first;
            }
        }

        return string();
    }

    int TZMap::getOffset(const string& tzId) {
        return this->id_attrib_map[tzId].offset;
    }

    string TZMap::getName(const string& tzId) {
        return this->id_attrib_map[tzId].name;
    }

    void TZMap::str(ostream& out) const {
        for (auto i : this->id_attrib_map) {
            out << i.first << " : " << i.second.name << " : ";
            TZ::offsetAsString(out, i.second.offset);
            out << uICAL::endl;
        }
    }
}
