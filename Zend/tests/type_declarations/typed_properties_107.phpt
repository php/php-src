--TEST--
Assigning stringable object to static string property
--FILE--
<?php

class Test1 {
    public static $ref;
}
class Test2 {
    public string $str = "str";
}
class Test3 {
    public function __toString() {
        $x = "foo";
        return $x . "bar";
    }
}

$test2 = new Test2;
Test1::$ref =& $test2->str;
Test1::$ref = new Test3;
var_dump(Test1::$ref);

?>
--EXPECTF--
Deprecated: Implicit conversion from Test3 to string for reference held by property Test2::$str of type string is deprecated in %s on line %d
string(6) "foobar"
