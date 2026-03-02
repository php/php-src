--TEST--
Bug #79900: Memory leaks reported if GC disabled at runtime
--FILE--
<?php

gc_disable();
$obj = new stdClass;
$obj->obj = $obj;

?>
===DONE===
--EXPECT--
===DONE===
