--TEST--
GH-10168: Assign with prop ref
--FILE--
<?php

class Test {
    static ?Test $test;

    function __destruct() {
        $GLOBALS['a'] = null;
    }
}

$a = new Test;
Test::$test = &$a;
var_dump($a = new Test);

?>
--EXPECT--
object(Test)#2 (0) {
}
