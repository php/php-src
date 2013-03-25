--TEST--
Bug #26077 (Memory leaks when creating an instance of an object)
--FILE--
<?php
class foo {} new foo();
?>
===DONE===
--EXPECT--
===DONE===
