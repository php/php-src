--TEST--
Static variable can override global
--FILE--
<?php

function foo() {
    global $a;
    $a = 42;
    var_dump($a);
    static $a = 41;
    var_dump($a);
}

foo();
var_dump($a);
foo();
var_dump($a);

?>
--EXPECT--
int(42)
int(41)
int(42)
int(42)
int(41)
int(42)
