--TEST--
GH-24712 (Use-after-free in foreach by reference when destructor modifies the loop variable)
--FILE--
<?php
class C {
    function __destruct() {
        $GLOBALS['a'] = null;
    }
}
$a = new C;
foreach ([1] as &$a);
var_dump($a);
?>
--EXPECT--
NULL
