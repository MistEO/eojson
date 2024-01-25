<div align="center">

# meojson

现代化的全平台 Json/Json5 解析/生成器，Header-only，并附带大量语法糖！

A modern all-platform Json/Json5 parser/serializer, which is header-only and contains lots of syntactic sugar!

</div>

[中文](./README.md)

## Usage

- Include the header file in your project, and enjoy it!

```cpp
#include "json.hpp"
```

- If you want to parse JSON5, please include `json5.hpp`

```cpp
#include "json5.hpp"
```

- meojson only depends on STL, but requires c++17 standard

## Sample

### Parsing

```cpp
/***
 * from sample/sample.cpp
***/
#include <iostream>
#include "json.hpp"

void parsing()
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
    }
}
    )";

    auto ret = json::parse(content);

    if (!ret) {
        std::cerr << "Parsing failed" << std::endl;
        return;
    }
    json::value& value = ret.value(); // you can use rvalues if needed, like
                               // `auto value = std::move(ret).value();`
    // Output: meojson
    std::cout << value["repo"].as_string() << std::endl;

    /* Output:
        ChingCdesu's homepage: https://github.com/ChingCdesu
        MistEO's homepage: https://github.com/MistEO
    */
    for (auto&& [name, homepage] : value["author"].as_object()) {
        std::cout << name << "'s homepage: " << homepage.as_string() << std::endl;
    }

    // Output: abc
    std::string str = (std::string)value["str"];    // it is equivalent to `value["str"].as_string()`
    std::cout << str << std::endl;

    // Output: 3.141600
    double num = value["num"].as_double();          // similarly, you can use `(double)value["num"]`
    std::cout << num << std::endl;

    // Output: default_value
    std::string get = value.get("maybe_exists", "default_value");
    std::cout << get << std::endl;

    // Output: you found me!
    std::string nested_get = value.get("A_obj", "B_arr", 1, "C_str", "default_value");
    std::cout << nested_get << std::endl;

    // Output: 1, 2, 3
    // If the "list" is not an array or not exists, it will be a invalid optional;
    auto opt = value.find<json::array>("list");
    if (opt) {
        auto& arr = opt.value();
        for (auto&& elem : arr) {
            std::cout << elem.as_integer() << std::endl;
        }
    }
    // more examples, it will output 3.141600
    auto opt_n = value.find<double>("num");
    if (opt_n) {
        std::cout << opt_n.value() << std::endl;
    }
    // If you use the `find` without template argument, it will return a `std::optional<json::value>`
    auto opt_v = value.find("not_exists");
    std::cout << "Did we find the \"not_exists\"? " << opt_v.has_value() << std::endl;

    bool is_vec = value["list"].is<std::vector<int>>();

    std::vector<int> to_vec = value["list"].as_collection<int>();
    to_vec = (std::vector<int>)value["list"];       // same as above
    to_vec = value["list"].as<std::vector<int>>();  // same as above

    // Output: 1, 2, 3
    for (auto&& i : to_vec) {
        std::cout << i << std::endl;
    }

    std::list<int> to_list = value["list"].as_collection<int, std::list>();
    to_list = (std::list<int>)value["list"];        // same as above
    to_list = value["list"].as<std::list<int>>();   // same as above

    std::set<int> to_set = value["list"].as_collection<int, std::set>();
    to_set = (std::set<int>)value["list"];          // same as above
    to_set = value["list"].as<std::set<int>>();     // same as above
    
    bool is_map = value["author"].is<std::map<std::string, std::string>>();
    
    std::map<std::string, std::string> to_map = value["author"].as_map<std::string>();
    to_map = (std::map<std::string, std::string>)value["author"];       // same as above
    to_map = value["author"].as<std::map<std::string, std::string>>();  // same as above

    auto to_hashmap = value["author"].as_map<std::string, std::unordered_map>();
    to_hashmap = (std::unordered_map<std::string, std::string>)value["author"];     // same as above
    to_hashmap = value["author"].as<std::unordered_map<std::string, std::string>>();// same as above

    // Output: "literals"
    using namespace json::literals;
    auto val = "{\"hi\":\"literals\"}"_json;
    std::cout << val["hi"] << std::endl;
}
```

### Parsing Json5

```cpp
/***
 * from sample/json5_parse.cpp
***/
#include <iostream>
#include "json5.hpp"

void parsing()
{
    std::string_view content = R"(
// It's a Json5 content
{
  名字: "MistEO",                  /* Key's quotes can be omitted */
  😊: '😄',                       // Emoji can be used as a key
  thanks: 'ありがとう',             /* Single quotes can also be used as strings */
  \u006Bey: ['value',],            // Normal characters and escapes can be mixed
  inf: +Infinity, nan: NaN,        // Numbers can start with '+'
  fractional: .3, integer: 42.,    // Allowed to start or end with decimal point
  byte_max: 0xff,                  // Supports hexadecimal number,
  light_speed: +3e8,               // and scientific notation
}
)";
    auto ret = json::parse5(content);
    if (!ret) {
        std::cerr << "Parsing failed" << std::endl;
        return;
    }
    json::value& value = ret.value(); // you can use rvalues if needed, like
                               // `auto value = std::move(ret).value();`

    // Output: MistEO
    std::cout << value["名字"] << std::endl;
    // Output: value
    std::string str = (std::string)value["key"][0];
    std::cout << str << std::endl;
    
    // for more json::value usage, please refer to sample.cpp
}
```

### Serializing

```cpp
/***
 * from sample/sample.cpp
***/
#include <iostream>
#include "json.hpp"

void serializing()
{
    json::value root;

    root["hello"] = "meojson";
    root["Pi"] = 3.1416;

    root["obj"] = {
        { "obj_key1", "Hi" },
        { "obj_key2", 123 },
        { "obj_key3", true },
    };
    root["obj"].emplace("obj_key4", 789);

    root["obj"].emplace("obj_key5", json::object { { "key4 child", "i am object value" } });
    root["another_obj"]["child"]["grand"] = "i am grand";

    // take union
    root["obj"] |= json::object {
        { "obj_key6", "i am string" },
        { "obj_key7", json::array { "i", "am", "array" } },
    };

    root["arr"] = json::array { 1, 2, 3 };
    root["arr"].emplace(4);
    root["arr"].emplace(5);
    root["arr"] += json::array { 6, 7 };

    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    root["arr from vec"] = vec;

    std::set<std::string> set = { "a", "bb\n\nb", "cc\t" };
    root["arr from set"] = set;

    std::map<std::string, int> map {
        { "key1", 1 },
        { "key2", 2 },
    };
    root["obj from map"] = map;

    std::vector<std::list<std::set<int>>> complex { { { 1, 2, 3 }, { 4, 5 } }, { { 6 }, { 7, 8 } } };
    root["complex"] = json::serialize<false>(complex);

    std::map<std::string, std::map<int, std::vector<double>>> more_complex {
        { "key1", { { 1, { 0.1, 0.2 } }, { 2, { 0.2, 0.3 } } } },
        { "key2", { { 3, { 0.4 } }, { 4, { 0.5, 0.6, 0.7 } } } },
    };
    // the "std::map<int, xxx>" cannot be converted to json because the key is "int",
    // you can set the template parameter "loose" of "serialize" to true, which will make a more relaxed conversion.
    root["more_complex"] = json::serialize<true>(more_complex);

    std::cout << root << std::endl;

    std::ofstream ofs("meo.json");
    ofs << root;
    ofs.close();
}
```

### JSONization

```c++
// if you are using MSVC, please add "/Zc:preprocessor" to your project
// if you are using AppleClang, please add "-Wno-gnu-zero-variadic-macro-arguments" to your project
void test_jsonization()
{
    struct MyStruct
    {
        std::vector<int> vec;
        std::map<std::string, int> map;
        int i = 0;
        double d = 0;

        // MEO_OPT means the var is optional
        // and can still be read even if the field doesn't exist in the input.
        MEO_JSONIZATION(vec, map, MEO_OPT i, d);
    };

    MyStruct a;
    a.vec = { 1, 2, 3 };
    a.map = { { "key", 5 } };
    a.i = 100;
    a.d = 0.5;

    json::value dumps = a;

    // output: { "d" : 0.500000, "i" : 100, "map" : { "key" : 5 }, "vec" : [ 1, 2, 3 ] }
    std::cout << dumps << std::endl;

    dumps.erase("i")
    // output: { "d" : 0.500000, "map" : { "key" : 5 }, "vec" : [ 1, 2, 3 ] }
    std::cout << dumps << std::endl;

    // MEO_OPT means the var is optional
    // and can still be read even if the field doesn't exist in the input.
    MyStruct b(dumps);

    // output: { "d" : 0.500000, "i" : 0, "map" : { "key" : 5 }, "vec" : [ 1, 2, 3 ] }
    // "i" is 0 because we erase "i" from the dumps
    std::cout << json::value(b) << std::endl;
}
```
