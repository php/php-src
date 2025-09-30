--TEST--
GH-19999 (GC refcount assertion failure when destructor modifies variable during concat)
--FILE--
<?php
class Test {
    function __destruct() {
        global $a;
        $a = null;
    }
}

$a = [new Test];
$a .= $a;
?>
--EXPECTF--
Warning: Array to string conversion in %s on line %d
