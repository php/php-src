--TEST--
GH-10168: Assign
--FILE--
<?php

class Test {
    function __destruct() {
        $GLOBALS['a'] = null;
    }
}

$a = new Test;
var_dump($a = new Test);

?>
--EXPECT--
object(Test)#2 (0) {
}
