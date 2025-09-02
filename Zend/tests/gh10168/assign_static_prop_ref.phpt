--TEST--
GH-10168: Assign static prop ref
--FILE--
<?php

class Test {
    static ?Test $test;

    function __destruct() {
        Test::$test = null;
    }
}

Test::$test = new Test;
$tmp = new Test;
var_dump(Test::$test = &$tmp);

?>
--EXPECT--
NULL
