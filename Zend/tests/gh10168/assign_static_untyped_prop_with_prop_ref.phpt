--TEST--
GH-10168: Assign static prop with prop ref
--FILE--
<?php

class Test {
    static $test;
    static ?Test $test2;

    function __destruct() {
        Test::$test = null;
    }
}

Test::$test = new Test;
Test::$test2 = &Test::$test;
var_dump(Test::$test = new Test);

?>
--EXPECT--
object(Test)#2 (0) {
}
