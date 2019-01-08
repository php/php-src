--TEST--
References to typed properties with undefined classes
--FILE--
<?php

class Test1 {
    public Foobar $prop;
    public int $prop2;
}

$test = new Test1;
$test->prop2 = 123;
$ref =& $test->prop2;
try {
    $test->prop =& $ref;
} catch (Error $e) {
    echo $e, "\n";
}

class Test2 {
    public ?Foobar $prop;
    public ?int $prop2;
}

$test = new Test2;
$test->prop2 = null;
$ref =& $test->prop2;
try {
    $test->prop =& $ref;
} catch (Error $e) {
    echo $e, "\n";
}

?>
--EXPECTF--
Error: Class Foobar must be loaded when used by reference for property type in %s:%d
Stack trace:
#0 {main}

Next TypeError: Reference with value of type int held by property Test1::$prop2 of type int is not compatible with property Test1::$prop of type Foobar in %s:%d
Stack trace:
#0 {main}
Error: Class Foobar must be loaded when used by reference for property type in %s:%d
Stack trace:
#0 {main}
