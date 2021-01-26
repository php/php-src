--TEST--
unset() CV 10 (unset() of global variable in ArrayObject::offsetUnset($GLOBALS))
--FILE--
<?php
/* This is working on a copy of $GLOBALS, so nothing interesting happens here. */
$a = new ArrayObject($GLOBALS);
$x = "ok\n";
echo $x;
$a->offsetUnset('x');
echo $x;
echo "ok\n";
?>
--EXPECTF--
ok

Warning: Undefined array key "x" in %s on line %d
ok
ok
