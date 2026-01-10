--TEST--
GH-10168: Assign static prop ref with prop ref
--FILE--
<?php

class Test {
    static ?Test $test;
    static ?Test $test2;

    function __destruct() {
        Test::$test = null;
    }
}

Test::$test = new Test;
Test::$test2 = &Test::$test;
$tmp = new Test;
var_dump(Test::$test = &$tmp);

?>
--EXPECT--
object(Test)#2 (0) {
}
