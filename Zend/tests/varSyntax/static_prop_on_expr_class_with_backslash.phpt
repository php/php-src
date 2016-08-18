--TEST--
Static property on constexpr class with leading backslash
--FILE--
<?php

class A {
    public static $b = 42;
}
var_dump(('\A' . (string) '')::$b);

?>
--EXPECT--
int(42)
