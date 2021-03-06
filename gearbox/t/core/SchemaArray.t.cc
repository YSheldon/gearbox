// Copyright (c) 2012, Yahoo! Inc.  All rights reserved.
// Copyrights licensed under the New BSD License. See the accompanying LICENSE file for terms.

#include <tap/trivial.h>

#include <gearbox/core/Json.h>
#include <gearbox/core/JsonSchema.h>
#include <gearbox/core/logger.h>

using namespace Gearbox;

int main() {
    TEST_START(56);
    log_init("./unit.conf");

    JsonSchema s;
    Json j;
    j.setSchema(&s);

    // empty object, no  properties
    s.parse("{\"type\":\"array\"}");

    NOTHROW( j.parse("[]") );
    NOTHROW( j.parse("[1,2,3]") );

    // dont allow additional items
    s.parse("{\"type\":\"array\",\"additionalProperties\":false}");
    
    NOTHROW( j.parse("[]") );
    THROWS( j.parse("[1]"), "Json Exception: out of bounds: schema does not allow for any elements in array" );
    
    s.parse("{\"type\":\"array\",\"additionalProperties\":false,\"items\":{\"type\":\"integer\"}}");
    NOTHROW( j.parse("[]") );
    NOTHROW( j.parse("[1]") );
    NOTHROW( j.parse("[1,2]") );
    NOTHROW( j.parse("[1,2,3]") );
    NOTHROW( j.parse("[1,2,3,4]") );
    THROWS( j.parse("[null]"), "Json Exception: schema does not allow for type \"null\" at: [0]" );
    
    s.parse("{\"type\":\"array\",\"additionalProperties\":false,\"items\":[{\"type\":\"integer\"},{\"type\":\"integer\"},{\"type\":\"integer\"},{\"type\":\"integer\"}]}");
    THROWS( j.parse("[]"), "Json Exception: array size 0 does not match schema required size of tuple type 4" );
    THROWS( j.parse("[1]"), "Json Exception: array size 1 does not match schema required size of tuple type 4" );
    THROWS( j.parse("[1,2]"), "Json Exception: array size 2 does not match schema required size of tuple type 4" );
    THROWS( j.parse("[1,2,3]"), "Json Exception: array size 3 does not match schema required size of tuple type 4" );
    NOTHROW( j.parse("[1,2,3,4]") );
    THROWS( j.parse("[1,2,3,4,5]"), "Json Exception: out of bounds: schema does not allow for elements at index 4" );
    THROWS( j.parse("[null]"), "Json Exception: schema does not allow for type \"null\" at: [0]" );
    

    // tuple type with unbounded additionalProperties
    s.parse("{\"type\":\"array\",\"items\":[{\"type\":\"integer\"},{\"type\":\"integer\"},{\"type\":\"integer\"},{\"type\":\"integer\"}]}");

    NOTHROW( j.parse("[]") );
    NOTHROW( j.parse("[1]") );
    NOTHROW( j.parse("[1,2]") );
    NOTHROW( j.parse("[1,2,3]") );
    NOTHROW( j.parse("[1,2,3,4]") );
    NOTHROW( j.parse("[1,2,3,4,5]") );
    NOTHROW( j.parse("[1,2,3,4,\"string\",true, 1.1,3,5]") );

    // tuple type with type'd additionalProperties
    s.parse("{\"type\":\"array\",\"additionalProperties\":{\"type\":\"integer\"},\"items\":[{\"type\":\"integer\"},{\"type\":\"integer\"},{\"type\":\"integer\"},{\"type\":\"integer\"}]}");

    NOTHROW( j.parse("[]") );
    NOTHROW( j.parse("[1]") );
    NOTHROW( j.parse("[1,2]") );
    NOTHROW( j.parse("[1,2,3]") );
    NOTHROW( j.parse("[1,2,3,4]") );
    NOTHROW( j.parse("[1,2,3,4,5]") );
    NOTHROW( j.parse("[1,2,3,4,5,6,7,8,9,10,11,12]") );
    THROWS( j.parse("[1,2,3,4,\"string\",true, 1.1,3,5]"), "Json Exception: schema does not allow for type \"string\" at: [4]" );


    // tuple type with unbounded additionalProperties, but with maxItems
    s.parse("{\"type\":\"array\",\"items\":[{\"type\":\"integer\"},{\"type\":\"integer\"}],\"maxItems\":6}");

    NOTHROW( j.parse("[]") );
    NOTHROW( j.parse("[1]") );
    NOTHROW( j.parse("[1,2]") );
    NOTHROW( j.parse("[1,2,3]") );
    NOTHROW( j.parse("[1,2,3,4]") );
    NOTHROW( j.parse("[1,2,3,4,5]") );
    NOTHROW( j.parse("[1,2,3,4,\"string\",true]") );
    THROWS( j.parse("[1,2,3,4,\"string\",true, 1.1,3]"), "Json Exception: array size is larger than schema maximum size 6" );

    // tuple type with unbounded additionalProperties, but with maxItems
    s.parse("{\"type\":\"array\",\"items\":[{\"type\":\"integer\"},{\"type\":\"integer\"}],\"maxItems\":6,\"minItems\":3}");

    THROWS( j.parse("[]"), "Json Exception: array size 0 is less than schema minimum size 3" );
    THROWS( j.parse("[1]"), "Json Exception: array size 1 is less than schema minimum size 3" );
    THROWS( j.parse("[1,2]"), "Json Exception: array size 2 is less than schema minimum size 3" );
    NOTHROW( j.parse("[1,2,3]") );
    NOTHROW( j.parse("[1,2,3,4]") );
    NOTHROW( j.parse("[1,2,3,4,5]") );
    NOTHROW( j.parse("[1,2,3,4,\"string\",true]") );
    THROWS( j.parse("[1,2,3,4,\"string\",true, 1.1,3]"), "Json Exception: array size is larger than schema maximum size 6" );

    s.parse("{\"type\":\"array\",\"items\":{\"type\":\"array\",\"items\":{\"type\":\"integer\"}}}");
    
    NOTHROW( j.parse("[]") );
    NOTHROW( j.parse("[[]]") );
    NOTHROW( j.parse("[[],[]]") );
    NOTHROW( j.parse("[[],[],[]]") );
    THROWS( j.parse("[{}]"), "Json Exception: schema does not allow for type \"object\" at: [0]" );
    THROWS( j.parse("[1]"), "Json Exception: schema does not allow for type \"integer\" at: [0]" );
    NOTHROW( j.parse("[[1,2,3],[4,5,6],[7,8,9]]") );
    THROWS(  j.parse("[[1,2,3],[4,5,6],[7,8,null]]"), "Json Exception: schema does not allow for type \"null\" at: [2][2]" );

    TEST_END;
}
