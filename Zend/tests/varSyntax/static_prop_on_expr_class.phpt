--TEST--
Accessing a static property on a statically evaluable class expression
--FILE--
<?php

class A {
    public static $b = 42;
}
var_dump(('A' . (string) '')::$b);

?>
--EXPECT--
int(42)
