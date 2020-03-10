--TEST--
Bug #41919 (crash in string to array conversion)
--FILE--
<?php
$foo="50";
$foo[3]->bar[1] = "bang";

echo "ok\n";
?>
--EXPECTF--
Warning: Uninitialized string offset 3 in %s on line %d

Fatal error: Uncaught Error: Attempt to modify property "bar" on string in %s:%d
Stack trace:
#0 {main}
  thrown in %sbug41919.php on line %d
