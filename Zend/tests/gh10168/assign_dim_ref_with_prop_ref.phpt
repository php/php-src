--TEST--
GH-10168: Assign dim ref with prop ref
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
$tmp = new Test;
var_dump($a[0] = &$tmp);

?>
--EXPECT--
object(Test)#2 (0) {
}
