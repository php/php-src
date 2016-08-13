--TEST--
Bug #41919 (crash in string to array conversion)
--FILE--
<?php
$foo="50";
$foo[3]->bar[1] = "bang";

echo "ok\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot use string offset as an object in %sbug41919.php:%d
Stack trace:
#0 {main}
  thrown in %sbug41919.php on line %d
