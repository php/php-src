--TEST--
Invalid string offset use where the actual use may be optimized away
--FILE--
<?php
function test() {
    $a = "";
    $c = array(&$a[0]);
}
test();
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot create references to/from string offsets in %s:%d
Stack trace:
#0 %s(%d): test()
#1 {main}
  thrown in %s on line %d
