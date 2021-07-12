--TEST--
GC of object property table (order variation)
--FILE--
<?php
function test() {
    $o1 = new stdClass;
    $o2 = new stdClass;
    $a = ['prop' => $o2];
    $o = $o1;
    $o2->a = (object) $a;
}
test();
?>
===DONE===
--EXPECT--
===DONE===
