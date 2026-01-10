--TEST--
GH-10168: Assign dim with prop ref
--FILE--
<?php

class Test {
    static ?Test $test;

    function __destruct() {
        $GLOBALS['a'] = null;
    }
}

$a = [new Test];
Test::$test = &$a[0];
var_dump($a[0] = new Test);

?>
--EXPECT--
object(Test)#2 (0) {
}
