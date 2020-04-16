--TEST--
Bug #79247 (Garbage collecting variant objects segfaults)
--SKIPIF--
<?php
if (!extension_loaded('com_dotnet')) die('skip com_dotnet extension not available');
?>
--FILE--
<?php
$keep = new variant(null);
var_dump(gc_collect_cycles());
?>
--EXPECT--
int(0)
