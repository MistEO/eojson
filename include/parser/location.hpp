#pragma once

#include "../common/types.hpp"

namespace json
{

namespace location
{

template <typename string_t = default_string_t>
using json_path = std::vector<std::variant<string_t, size_t>>;

struct position
{
    size_t offset = 0;
    size_t row = 0;
    size_t column = 0;
};

struct range
{
    position start, end;
};

template <typename string_t = default_string_t>
struct visitor
{
    virtual ~visitor() = default;
    virtual void property(
        const string_t& key,
        const position& start,
        const position& end,
        const json_path<string_t>& path);
    virtual void value(
        const basic_value<string_t>& value,
        const position& start,
        const position& end,
        const json_path<string_t>& path);
    virtual void object_enter(const position& start, const json_path<string_t>& path);
    virtual void
        object_leave(const position& start, const position& end, const json_path<string_t>& path);
    virtual void array_enter(const position& start, const json_path<string_t>& path);
    virtual void
        array_leave(const position& start, const position& end, const json_path<string_t>& path);
};

template <typename string_t = default_string_t>
struct location_info
{
    struct object_entry
    {
        range property;
        location_info info;
    };

    using parse_array_info = std::vector<location_info>;
    using parse_object_info = std::map<string_t, object_entry>;

    range _self;
    std::variant<std::monostate, parse_array_info, parse_object_info> _info;

    bool is_arr() const;
    bool is_obj() const;
    parse_array_info& arr();
    const parse_array_info& arr() const;
    parse_object_info& obj();
    const parse_object_info& obj() const;
};

template <typename string_t = default_string_t>
class location_info_generator : public visitor<string_t>
{
public:
    using info_t = location_info<string_t>;

    const info_t& info() const { return _info; }

private:
    info_t _info;
    std::vector<info_t*> _process;
    typename info_t::object_entry* _obj_entry = nullptr;

    void before_value();
    void after_value();

    void property(
        const string_t& key,
        const position& start,
        const position& end,
        const json_path<string_t>& path) override;
    void value(
        const basic_value<string_t>& value,
        const position& start,
        const position& end,
        const json_path<string_t>& path) override;
    void object_enter(const position& start, const json_path<string_t>& path) override;
    void object_leave(const position& start, const position& end, const json_path<string_t>& path)
        override;
    void array_enter(const position& start, const json_path<string_t>& path) override;
    void array_leave(const position& start, const position& end, const json_path<string_t>& path)
        override;

private:
    info_t& cur() { return *_process.back(); }
};

template <typename string_t>
inline void visitor<string_t>::property(
    [[maybe_unused]] const string_t& key,
    [[maybe_unused]] const position& start,
    [[maybe_unused]] const position& end,
    [[maybe_unused]] const json_path<string_t>& path)
{
}

template <typename string_t>
inline void visitor<string_t>::value(
    [[maybe_unused]] const basic_value<string_t>& value,
    [[maybe_unused]] const position& start,
    [[maybe_unused]] const position& end,
    [[maybe_unused]] const json_path<string_t>& path)
{
}

template <typename string_t>
inline void visitor<string_t>::object_enter(
    [[maybe_unused]] const position& start,
    [[maybe_unused]] const json_path<string_t>& path)
{
}

template <typename string_t>
inline void visitor<string_t>::object_leave(
    [[maybe_unused]] const position& start,
    [[maybe_unused]] const position& end,
    [[maybe_unused]] const json_path<string_t>& path)
{
}

template <typename string_t>
inline void visitor<string_t>::array_enter(
    [[maybe_unused]] const position& start,
    [[maybe_unused]] const json_path<string_t>& path)
{
}

template <typename string_t>
inline void visitor<string_t>::array_leave(
    [[maybe_unused]] const position& start,
    [[maybe_unused]] const position& end,
    [[maybe_unused]] const json_path<string_t>& path)
{
}

template <typename string_t>
bool location_info<string_t>::is_arr() const
{
    return _info.index() == 1;
}

template <typename string_t>
bool location_info<string_t>::is_obj() const
{
    return _info.index() == 2;
}

template <typename string_t>
typename location_info<string_t>::parse_array_info& location_info<string_t>::arr()
{
    return std::get<1>(_info);
}

template <typename string_t>
const typename location_info<string_t>::parse_array_info& location_info<string_t>::arr() const
{
    return std::get<1>(_info);
}

template <typename string_t>
typename location_info<string_t>::parse_object_info& location_info<string_t>::obj()
{
    return std::get<2>(_info);
}

template <typename string_t>
const typename location_info<string_t>::parse_object_info& location_info<string_t>::obj() const
{
    return std::get<2>(_info);
}

template <typename string_t>
inline void location_info_generator<string_t>::before_value()
{
    if (_process.empty()) {
        _process.push_back(&_info);
    }
    else {
        switch (cur()._info.index()) {
        case 0:
            throw "value inside value";
        case 1:
            cur().arr().push_back({});
            _process.push_back(&cur().arr().back());
            break;
        case 2:
            if (!_obj_entry) {
                throw "obj_entry is null while _info is object";
            }
            _process.push_back(&_obj_entry->info);
            _obj_entry = nullptr;
            break;
        }
    }
}

template <typename string_t>
inline void location_info_generator<string_t>::after_value()
{
    _process.pop_back();
}

template <typename string_t>
inline void location_info_generator<string_t>::property(
    const string_t& key,
    const position& start,
    const position& end,
    const json_path<string_t>& path)
{
    std::ignore = key;
    std::ignore = start;
    std::ignore = end;
    std::ignore = path;

    if (cur()._info.index() != 2) {
        throw "property called without object_enter";
    }
    auto& sub = cur().obj()[key];
    sub.property = { start, end };
    _obj_entry = &sub;
}

template <typename string_t>
inline void location_info_generator<string_t>::value(
    const basic_value<string_t>& value,
    const position& start,
    const position& end,
    const json_path<string_t>& path)
{
    std::ignore = value;
    std::ignore = path;

    before_value();
    cur()._self = { start, end };
    cur()._info = std::monostate {};
    after_value();
}

template <typename string_t>
inline void location_info_generator<string_t>::object_enter(
    const position& start,
    const json_path<string_t>& path)
{
    std::ignore = start;
    std::ignore = path;

    before_value();
    cur()._info = typename location_info<string_t>::parse_object_info {};
}

template <typename string_t>
inline void location_info_generator<string_t>::object_leave(
    const position& start,
    const position& end,
    const json_path<string_t>& path)
{
    std::ignore = path;

    cur()._self = { start, end };
    after_value();
}

template <typename string_t>
inline void location_info_generator<string_t>::array_enter(
    const position& start,
    const json_path<string_t>& path)
{
    std::ignore = start;
    std::ignore = path;

    before_value();
    cur()._info = typename location_info<string_t>::parse_array_info {};
}

template <typename string_t>
inline void location_info_generator<string_t>::array_leave(
    const position& start,
    const position& end,
    const json_path<string_t>& path)
{
    std::ignore = path;

    cur()._self = { start, end };
    after_value();
}

}

}
