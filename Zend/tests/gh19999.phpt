--TEST--
GH-19999 (GC refcount assertion via object destruction during concat assignment)
--FILE--
<?php
class Test {
    public function __destruct() {
        $GLOBALS['a'] = null;
    }
}
$a = [new Test];
$a .= $a;
var_dump($a);
?>
--EXPECTF--
Warning: Array to string conversion in %s on line %d
NULL
