--TEST--
Test get_debug_type() class reading
--FILE--
<?php

namespace Demo {
    class ClassInNamespace {

    }
}

namespace {
    class ClassInGlobal {

    }

    /* tests against an object type */
    echo get_debug_type(new ClassInGlobal()) . "\n";
    echo get_debug_type(new Demo\ClassInNamespace()) . "\n";

    /* scalars */
    echo get_debug_type("foo") . "\n";
    echo get_debug_type(false) . "\n";
    echo get_debug_type(true) . "\n";
    echo get_debug_type(1) . "\n";
    echo get_debug_type(1.1) . "\n";
    echo get_debug_type([]) . "\n";
    echo get_debug_type(null) . "\n";
    echo get_debug_type(fopen(__FILE__, 'r')) . "\n";

}

--EXPECT--
ClassInGlobal
Demo\ClassInNamespace
string
bool
bool
int
float
array
null
resource
