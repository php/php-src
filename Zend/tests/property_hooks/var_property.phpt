--TEST--
Var hooked property is implicitly public
--FILE--
<?php

class Test {
    var $prop { get => 42; }
}

$test = new Test();
var_dump($test->prop);

?>
--EXPECT--
int(42)
