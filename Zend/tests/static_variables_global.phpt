--TEST--
Global can override static variable
--FILE--
<?php

function foo() {
    static $a = 42;
    var_dump($a);
    global $a;
    $a = 41;
    var_dump($a);
}

foo();
foo();

?>
--EXPECT--
int(42)
int(41)
int(42)
int(41)
