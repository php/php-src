--TEST--
Make sure various ASSIGN_OBJ exceptions are not optimized away
--FILE--
<?php

class Test {
    public stdClass $x;
}

function test_invalid_prop_type() {
    $test = new Test;
    $test->x = "";
}
function test_invalid_prop_name(string $name) {
    $test = new stdClass;
    $test->$name = null;
}
function test_invalid_obj_type($c) {
    if ($c) {
        $test = new stdClass;
    } else {
        $test = null;
    }
    $test->x = "";
}

try {
    test_invalid_prop_type();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    test_invalid_prop_name("\0");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test_invalid_obj_type(false);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot assign string to property Test::$x of type stdClass
Cannot access property starting with "\0"
Attempt to assign property "x" on null
