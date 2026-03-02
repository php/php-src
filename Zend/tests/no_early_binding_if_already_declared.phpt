--TEST--
No early binding should occur if the class is already declared
--FILE--
<?php
class A {}
class B extends A {}
include __DIR__ . '/no_early_binding_if_already_declared.inc';
?>
===DONE===
--EXPECT--
===DONE===
