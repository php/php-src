--TEST--
Duplicate class alias type at runtime
--FILE--
<?php

class A {}

class_alias('A', 'B');
function foo(): A&B {}

?>
===DONE===
--EXPECT--
===DONE===
