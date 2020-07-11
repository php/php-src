--TEST--
unset() CV 10 (unset() of global variable in ArrayObject::offsetUnset($GLOBALS))
--FILE--
<?php
$a = new ArrayObject($GLOBALS);
$x = "ok\n";
echo $x;
$a->offsetUnset('x');
echo $x;
echo "ok\n";
?>
--EXPECTF--
ok

Warning: Undefined variable $x in %s on line %d
ok
