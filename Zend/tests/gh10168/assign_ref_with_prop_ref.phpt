--TEST--
GH-10168: Assign ref with prop ref
--FILE--
<?php

class Test {
    static ?Test $test;

    function __destruct() {
        $GLOBALS['a'] = null;
    }
}

$a = new Test;
$tmp = new Test;
var_dump($a = &$tmp);

?>
--EXPECT--
NULL
