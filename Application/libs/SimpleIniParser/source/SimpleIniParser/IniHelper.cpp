/*
 * SimpleIniParser
 * Copyright (c) 2020 Nichole Mattera
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above 
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "IniHelper.hpp"
#include "IniStringHelper.hpp"

namespace simpleIniParser {
    bool IniHelper::findOption(const IniOption * obj, std::string term, bool caseSensitive, IniOptionType type, IniOptionSearchField field) {
        if (type != IniOptionType::Any && type != obj->type) {
            return false;
        }
        
        std::string fieldValue = "";
        if (field == IniOptionSearchField::Key) {
            fieldValue = (!caseSensitive) ? IniStringHelper::toupper_copy(obj->key) : obj->key;
        } else {
            fieldValue = (!caseSensitive) ? IniStringHelper::toupper_copy(obj->value) : obj->value;
        }

        return fieldValue == term;
    }

    bool IniHelper::findSection(const IniSection * obj, std::string term, bool caseSensitive, IniSectionType type) {
        if (type != IniSectionType::Any && type != obj->type) {
            return false;
        }

        std::string fieldValue = (!caseSensitive) ? IniStringHelper::toupper_copy(obj->value) : obj->value;
        return fieldValue == term;
    }
}


