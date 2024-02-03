#include <deque>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

#include "json.hpp"

void serializing();
void third_party_jsonization_1();
void third_party_jsonization_2();
void parsing();

int main()
{
    serializing();
    parsing();

    return 0;
}

void serializing()
{
    /* Here are some basic features: */

    json::value j;
    j["pi"] = 3.14;
    j["happy"] = true;
    j["answer"]["everything"] = 42;
    j["object"] = { { "currency", "USD" }, { "value", 42.99 } };

    /* And some interesting features: */

    std::set<int> set { 1, 2, 3 };
    j["set"] = set;

    // what a crazy type!
    std::unordered_map<std::string, std::list<std::map<std::string, std::deque<int>>>> map {
        { "key_1", { { { "inner_key_1", { 7, 8, 9 } } }, { { "inner_key_2", { 10 } } } } },
    };
    j["map"] = map;

    // output:
    // {"answer":{"everything":42},"happy":true,"map":{"key_1":[{"inner_key_1":[7,8,9]},{"inner_key_2":[10]}]},"object":{"currency":"USD","value":42.990000},"pi":3.140000,"set":[1,2,3]}
    std::cout << j << std::endl;

    /* Then, don’t blink, we changed it back! */

    double pi = (double)j["pi"];
    int answer = (int)j["answer"]["everything"];

    std::set<int> new_set = (std::set<int>)j["set"];
    // this crazy type again
    auto new_map = (std::unordered_map<std::string, std::list<std::map<std::string, std::deque<int>>>>)j["map"];

    /* However, for runtime json, we'd better check whether it can be converted first. */

    if (j["happy"].is<std::vector<int>>()) {
        std::vector<int> vec = (std::vector<int>)j["happy"];
    }
    else {
        std::cout << "Oh my god, j[\"happy\"] is not an array." << std::endl;
        std::cout << "Fortunately, I checked it, otherwise it will crash!" << std::endl;
    }

    /* I guess you have understood, yes, **meojson** is not only a json library, but also a serialization library! */

    struct MyStruct
    {
        int x = 0;
        std::vector<double> vec;
        // how come it's always you!
        std::unordered_map<std::string, std::list<std::map<std::string, std::deque<int>>>> map;

        // then we add a little magic
        MEO_JSONIZATION(x, vec, map);
    };

    MyStruct mine;
    mine.vec.emplace_back(0.5);
    mine.map = { { "key_1", { { { "inner_key_1", { 7, 8, 9 } } }, { { "inner_key_2", { 10 } } } } } };

    // yes, it’s that intuitive and smooth!
    json::value j_mine = mine;

    // output: {"map":{"key_1":[{"inner_key_1":[7,8,9]},{"inner_key_2":[10]}]},"vec":[0.500000],"x":0}
    std::cout << j_mine << std::endl;

    // exactly, we can also change it back!
    MyStruct new_mine = (MyStruct)j_mine;

    /* Nested calls are also a no-brainer! */

    struct Outter
    {
        int outter_a = 10;
        std::vector<MyStruct> my_vec;

        MEO_JSONIZATION(outter_a, my_vec);
    };

    Outter outter;
    outter.my_vec.emplace_back(mine);

    json::value j_outter = outter;

    // output:
    // {"my_vec":[{"map":{"key_1":[{"inner_key_1":[7,8,9]},{"inner_key_2":[10]}]},"vec":[0.500000],"x":0}],"outter_a":10}
    std::cout << j_outter.to_string() << std::endl;

    // same deserialization
    Outter new_o = (Outter)j_outter;

    /* For optional fields, we can add `MEO_OPT` to it, so that when converting, if this fields does not exist in json,
     * it will be skipped. */

    struct OptionalFields
    {
        int a = 0;
        double b = 0;
        std::vector<int> c;

        MEO_JSONIZATION(a, MEO_OPT b, MEO_OPT c);
    };

    json::value ja = {
        { "a", 100 },
    };
    if (ja.is<OptionalFields>()) {
        OptionalFields var = (OptionalFields)ja;
        std::cout << var.a << std::endl;
    }

    third_party_jsonization_1();
    third_party_jsonization_2();

    /* And some trivial features: */

    // add elements to an array or object via `emplace`
    j["set"].emplace(10);
    j["object"].emplace("key3", "value3");

    // merge two arrays
    j["set"] += json::array { 11, 12 };

    // merge two objects
    j["object"] |= {
        { "key4", 4 },
        { "key5", false },
    };

    // to string
    std::string oneline = j.dumps();
    std::string format = j.dumps(4);

    // save to file
    std::ofstream ofs("meo.json");
    ofs << j;
    ofs.close();
}

struct ThirdPartyStruct
{
    int a = 100;
};

namespace json
{
template <>
class serialization<ThirdPartyStruct>
{
public:
    json::value to_json(const ThirdPartyStruct& t) const { return t.a; }
    bool check_json(const json::value& j) const { return j.is_number(); }
    bool from_json(const json::value& j, ThirdPartyStruct& out) const
    {
        out.a = j.as_integer();
        return true;
    }
};
} // namespace json

void third_party_jsonization_1()
{
    /* For third-party unhackable types, you need to implement `to_json`, `check_json`, `from_json` */

    // then you can use it as json
    ThirdPartyStruct third;
    json::value jthird = third;
    ThirdPartyStruct new_third = (ThirdPartyStruct)jthird;

    // or add to your sturcture
    struct Outter2
    {
        int outter2_a = 10;
        ThirdPartyStruct third;

        MEO_JSONIZATION(outter2_a, third);
    };
    Outter2 o_2;
}

void third_party_jsonization_2()
{
    /* If you don't like stupid invasive function, you can use `json::serialize` and `json::deserialize`
     * for more elegant conversion: */
    struct Serializer
    {
        json::value operator()(const ThirdPartyStruct& t) const { return t.a; }
    };
    struct Deserializer
    {
        bool operator()(const json::value& j, ThirdPartyStruct& t) const
        {
            if (!j.is_number()) return false;
            t.a = j.as_integer();
            return true;
        }
    };

    std::map<std::string, ThirdPartyStruct> third;
    third["key"] = { 100 };
    json::value jthird = json::serialize(third, Serializer {});

    std::cout << jthird << std::endl;

    std::map<std::string, ThirdPartyStruct> new_third;
    bool ret = json::deserialize(jthird, new_third, Deserializer {});
}

void parsing()
{
    /* Now let’s talk about parsing */

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
})";

    // it's a std::optional<json::value>
    auto ret = json::parse(content);

    if (!ret) {
        std::cerr << "Parsing failed" << std::endl;
        return;
    }
    json::value& value = *ret;

    // Output: meojson
    std::cout << (std::string)value["repo"] << std::endl;

    /* Output:
        ChingCdesu's homepage: https://github.com/ChingCdesu
        MistEO's homepage: https://github.com/MistEO
    */
    for (auto&& [name, homepage] : (json::object)value["author"]) {
        std::cout << name << "'s homepage: " << (std::string)homepage << std::endl;
    }
    // num = 3.141600
    double num = (double)value["num"];

    // get_value = "default_value"
    std::string get_value = value.get("maybe_exists", "default_value");
    std::cout << get_value << std::endl;

    /* Like most parsing libraries, this is boring and you don't want to look at this.
       So let me show you something interesting. */

    // what a magical `get`, you can continuously enter keys or pos!
    // nested_get = you found me!
    std::string nested_get = value.get("A_obj", "B_arr", 1, "C_str", "default_value");

    // `find` can help you find and check whether the type is correct
    // if there is no `num`, the opt_n will be std::nullopt
    auto opt_n = value.find<double>("num");
    if (opt_n) {
        // output: 3.141600
        std::cout << *opt_n << std::endl;
    }

    /* There are also a few tricks you've already seen with Serializing */

    bool is_vec = value["list"].is<std::vector<int>>();

    std::vector<int> to_vec = value["list"].as_collection<int>();
    to_vec = (std::vector<int>)value["list"];      // same as above
    to_vec = value["list"].as<std::vector<int>>(); // same as above

    // Output: 1, 2, 3
    for (auto&& i : to_vec) {
        std::cout << i << std::endl;
    }

    std::list<int> to_list = value["list"].as_collection<int, std::list>();
    to_list = (std::list<int>)value["list"];      // same as above
    to_list = value["list"].as<std::list<int>>(); // same as above

    std::set<int> to_set = value["list"].as_collection<int, std::set>();
    to_set = (std::set<int>)value["list"];      // same as above
    to_set = value["list"].as<std::set<int>>(); // same as above

    bool is_map = value["author"].is<std::map<std::string, std::string>>();

    std::map<std::string, std::string> to_map = value["author"].as_map<std::string>();
    to_map = (std::map<std::string, std::string>)value["author"];      // same as above
    to_map = value["author"].as<std::map<std::string, std::string>>(); // same as above

    auto to_hashmap = value["author"].as_map<std::string, std::unordered_map>();
    to_hashmap = (std::unordered_map<std::string, std::string>)value["author"];      // same as above
    to_hashmap = value["author"].as<std::unordered_map<std::string, std::string>>(); // same as above

    /* And... some useless literal syntax */

    // Output: "literals"
    using namespace json::literals;
    auto val = "{\"hi\":\"literals\"}"_json;
    std::cout << val["hi"] << std::endl;
}
