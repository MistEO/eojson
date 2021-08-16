#include "json_object.h"

#include "json_value.h"
#include "json_aux.h"


json::object::object(const raw_object& raw_obj)
    : _object_data(raw_obj)
{
    ;
}

json::object::object(raw_object&& raw_obj)
    : _object_data(std::move(raw_obj))
{
    ;
}

json::object::object(std::initializer_list<raw_object::value_type> init_list)
{
    for (const auto& [key, val] : init_list)
    {
        emplace(key, val);
    }
}

bool json::object::exist(const std::string& key) const
{
    return _object_data.find(key) != _object_data.cend();
}

const json::value& json::object::at(const std::string& key) const
{
    return _object_data.at(key);
}

void json::object::clear() noexcept
{
    _object_data.clear();
}

bool json::object::earse(const std::string& key)
{
    return _object_data.erase(key) > 0 ? true : false;
}

const std::string json::object::to_string() const
{
    std::string str = "{";
    for (const auto& [key, val] : _object_data)
    {
        str += "\"" + unescape_string(key) + "\":" + val.to_string() + ",";
    }
    if (str.back() == ',')
    {
        str.pop_back();
    }
    str += "}";
    return str;
}

const std::string json::object::format(std::string shift_str, size_t basic_shift_count) const
{
    std::string shift;
    for (size_t i = 0; i != basic_shift_count + 1; ++i)
    {
        shift += shift_str;
    }

    std::string str = "{";
    for (const auto& [key, val] : _object_data)
    {
        str += "\n" + shift + "\"" + unescape_string(key) + "\": " + val.format(shift_str, basic_shift_count + 1) + ",";
    }
    if (str.back() == ',')
    {
        str.pop_back(); // pop last ','
    }

    str += '\n';
    for (size_t i = 0; i != basic_shift_count; ++i)
    {
        str += shift_str;
    }
    str += '}';
    return str;
}

const bool json::object::get(const std::string& key, bool default_value) const
{
    if (exist(key)) {
        json::value value = _object_data.at(key);
        if (value.is_boolean()) {
            return value.as_boolean();
        }
        else {
            return default_value;
        }
    }
    else {
        return default_value;
    }
}

const int json::object::get(const std::string& key, int default_value) const
{
    if (exist(key)) {
        json::value value = _object_data.at(key);
        if (value.is_number()) {
            return value.as_integer();
        }
        else {
            return default_value;
        }
    }
    else {
        return default_value;
    }
}

const long json::object::get(const std::string& key, long default_value) const
{
    if (exist(key)) {
        json::value value = _object_data.at(key);
        if (value.is_number()) {
            return value.as_long();
        }
        else {
            return default_value;
        }
    }
    else {
        return default_value;
    }
}

const unsigned long json::object::get(const std::string& key, unsigned default_value) const
{
    if (exist(key)) {
        json::value value = _object_data.at(key);
        if (value.is_number()) {
            return value.as_unsigned_long();
        }
        else {
            return default_value;
        }
    }
    else {
        return default_value;
    }
}

const long long json::object::get(const std::string& key, long long default_value) const
{
    if (exist(key)) {
        json::value value = _object_data.at(key);
        if (value.is_number()) {
            return value.as_long_long();
        }
        else {
            return default_value;
        }
    }
    else {
        return default_value;
    }
}

const unsigned long long json::object::get(const std::string& key, unsigned long long default_value) const
{
    if (exist(key)) {
        json::value value = _object_data.at(key);
        if (value.is_number()) {
            return value.as_unsigned_long_long();
        }
        else {
            return default_value;
        }
    }
    else {
        return default_value;
    }
}

const float json::object::get(const std::string& key, float default_value) const
{
    if (exist(key)) {
        json::value value = _object_data.at(key);
        if (value.is_number()) {
            return value.as_float();
        }
        else {
            return default_value;
        }
    }
    else {
        return default_value;
    }
}

const double json::object::get(const std::string& key, double default_value) const
{
    if (exist(key)) {
        json::value value = _object_data.at(key);
        if (value.is_number()) {
            return value.as_double();
        }
        else {
            return default_value;
        }
    }
    else {
        return default_value;
    }
}

const long double json::object::get(const std::string& key, long double default_value) const
{
    if (exist(key)) {
        json::value value = _object_data.at(key);
        if (value.is_number()) {
            return value.as_long_double();
        }
        else {
            return default_value;
        }
    }
    else {
        return default_value;
    }
}

const std::string json::object::get(const std::string& key, std::string default_value) const
{
    if (exist(key)) {
        json::value value = _object_data.at(key);
        if (value.is_string()) {
            return value.as_string();
        }
        else {
            return default_value;
        }
    }
    else {
        return default_value;
    }
}

const std::string json::object::get(const std::string& key, const char* default_value) const
{
    if (exist(key)) {
        json::value value = _object_data.at(key);
        if (value.is_string()) {
            return value.as_string();
        }
        else {
            return default_value;
        }
    }
    else {
        return default_value;
    }
}

json::object::iterator json::object::begin() noexcept
{
    return _object_data.begin();
}

json::object::iterator json::object::end() noexcept
{
    return _object_data.end();
}

json::object::const_iterator json::object::cbegin() const noexcept
{
    return _object_data.cbegin();
}

json::object::const_iterator json::object::cend() const noexcept
{
    return _object_data.cend();
}

json::value& json::object::operator[](const std::string& key)
{
    return _object_data[key];
}

json::value& json::object::operator[](std::string&& key)
{
    return _object_data[std::move(key)];
}

// const raw_object &json::object::raw_data() const
// {
//     return _object_data;
// }

std::ostream& operator<<(std::ostream& out, const json::object& obj)
{
    // TODO: format output

    out << obj.to_string();
    return out;
}