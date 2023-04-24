--TEST--
GH-10168: Assign dim ref
--FILE--
<?php

class Test {
    function __destruct() {
        $GLOBALS['a'] = null;
    }
}

$a = [new Test];
$tmp = new Test;
var_dump($a[0] = &$tmp);

?>
--EXPECT--
object(Test)#2 (0) {
}
