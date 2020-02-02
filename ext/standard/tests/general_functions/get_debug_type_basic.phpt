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
    echo get_debug_type(1) . "\n";
    echo get_debug_type(1.1) . "\n";
    echo get_debug_type("foo") . "\n";
    echo get_debug_type(Test::class) . "\n";
}

--EXPECT--
ClassInGlobal
Demo\ClassInNamespace
integer
double
string
string
