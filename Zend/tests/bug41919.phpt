--TEST--
Bug #41919 (crash in string to array conversion)
--FILE--
<?php
$foo="50";
$foo[3]->bar[1] = "bang";

echo "ok\n";
?>
--EXPECTF--
Notice: Uninitialized string offset: 3 in %s on line %d

Warning: Attempt to modify property 'bar' of non-object in %s on line %d
ok
