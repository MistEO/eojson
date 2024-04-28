#include <iostream>

#include "json.hpp"

#include "location_test.h"

std::ostream& operator<<(std::ostream& os, const json::location::position& pos)
{
    return os << pos.offset << ":" << pos.row << ":" << pos.column;
}

std::ostream& operator<<(std::ostream& os, const json::location::json_path<std::string>& path)
{
    for (size_t i = 0; i < path.size(); i++) {
        const auto& p = path[i];
        if (i) {
            os << ".";
        }
        if (p.index() == 0) {
            os << std::get<0>(p);
        }
        else {
            os << std::get<1>(p);
        }
    }
    return os;
}

struct my_visitor : public json::location::visitor<std::string>
{
    virtual void property(
        const std::string& key,
        const json::location::position& start,
        const json::location::position& end,
        const json::location::json_path<std::string>& path)
    {
        std::cout << "property found: " << key << "\n  from " << start << "\n  to " << end
                  << "\n  at " << path << std::endl;
    }

    virtual void value(
        const json::basic_value<std::string>& value,
        const json::location::position& start,
        const json::location::position& end,
        const json::location::json_path<std::string>& path)
    {
        std::cout << "value found: " << value << "\n  from " << start << "\n  to " << end
                  << "\n  at " << path << std::endl;
        ;
    }

    virtual void object_enter(
        const json::location::position& start,
        const json::location::json_path<std::string>& path)
    {
        std::cout << "object enter\n  from " << start << "\n  at " << path << std::endl;
        ;
    }

    virtual void object_leave(
        const json::location::position& start,
        const json::location::position& end,
        const json::location::json_path<std::string>& path)
    {
        std::cout << "object leave\n  from " << start << "\n  to " << end << "\n  at " << path
                  << std::endl;
        ;
    }

    virtual void array_enter(
        const json::location::position& start,
        const json::location::json_path<std::string>& path)
    {
        std::cout << "array enter\n  from " << start << "\n  at " << path << std::endl;
        ;
    }

    virtual void array_leave(
        const json::location::position& start,
        const json::location::position& end,
        const json::location::json_path<std::string>& path)
    {
        std::cout << "array leave\n  from " << start << "\n  to " << end << "\n  at " << path
                  << std::endl;
        ;
    }
};

void dump_parse_info(
    std::ostream& os,
    const std::string& source,
    const json::location::location_info<std::string>& info,
    size_t indent = 0)
{
    auto extract = [&source](const json::location::range& range) {
        return source.substr(range.start.offset, range.end.offset - range.start.offset);
    };

    std::string indent_str(indent, ' ');
    switch (info._info.index()) {
    case 0:
        os << indent_str << "value " << info._self.start << '~' << info._self.end << ' '
           << extract(info._self) << std::endl;
        break;
    case 1:
        os << indent_str << "array " << info._self.start << '~' << info._self.end << " ["
           << std::endl;
        for (const auto& sub : info.arr()) {
            dump_parse_info(os, source, sub, indent + 2);
        }
        os << indent_str << "]" << std::endl;
        break;
    case 2:
        os << indent_str << "object " << info._self.start << '~' << info._self.end << " {"
           << std::endl;
        for (const auto& sub : info.obj()) {
            os << indent_str << "  property " << sub.second.property.start << '~'
               << sub.second.property.end << ' ' << extract(sub.second.property) << std::endl;
            dump_parse_info(os, source, sub.second.info, indent + 2);
        }
        os << indent_str << "}" << std::endl;
        break;
    }
}

bool location_test()
{
    std::string content = R"(
{
    "repo": "meojson",
    "author": {
        "MistEO": "https://github.com/MistEO",
        "ChingCdesu": "https://github.com/ChingCdesu"
    },
    "list": [ 1, 2, 3 ],
    "str": "abc\n123",
    "num": 3.1416,
    "A_obj": {
        "B_arr": [
            { "C_str": "i am a distraction" },
            { "C_str": "you found me!" }
        ]
    },
    "my_type": { "i": 99 }
})";

    std::cout << "\n/// custom visitor\n";

    json::parse(content, new my_visitor());

    std::cout << "\n/// location_info_generator\n";

    json::location::location_info_generator gen;

    json::parse(content, &gen);

    dump_parse_info(std::cout, content, gen.info());

    return true;
}
