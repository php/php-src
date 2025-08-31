--TEST--
Duplicate class alias type at runtime
--FILE--
<?php

class A {}
interface X {}

class_alias('A', 'B');
function foo(): (X&A)|(X&B) {}

?>
===DONE===
--EXPECT--
===DONE===
