--TEST--
Cannot capture the same variable twice without renaming
--FILE--
<?php
$a = 42;
$foo = new class use ($a, $a) {};
?>
--EXPECTF--
Fatal error: Redefinition of captured property $a in %s on line %d
