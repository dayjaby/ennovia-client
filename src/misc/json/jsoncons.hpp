#ifndef MISC_JSON_JSONCONS_HPP
#define MISC_JSON_JSONCONS_HPP
#include <misc/json/json.h>

namespace Json {
    template <class K, class A>
    struct Pair {
        K key;
        A value;
    };


    template <class K, class A>
    Pair<K,A> Val(K key,A val) { return Pair<K,A>{key,val}; }

    Json::Value Object() {
        return Json::Value(Json::objectValue);
    }

    template <class K, class A>
    Json::Value Object(Pair<K,A> v) {
        Json::Value val = Json::Value(Json::objectValue);//Object();
        val[v.key] = v.value;
        return val;
    }


    template <class K, class A, typename... Params>
    Json::Value Object(Pair<K,A> v, Params... args) {
        Json::Value val = Object(args...);
        val[v.key] = v.value;
        return val;
    }


    // Example usage: Json::Object{{"name","David"},{"Alter",23};
}


#endif // MISC_JSON_JSONCONS_HPP
