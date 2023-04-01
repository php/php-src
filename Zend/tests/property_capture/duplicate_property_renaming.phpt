--TEST--
Cannot rename two variables to the same captured property name
--FILE--
<?php
$a = 42;
$b = -1;
$foo = new class use ($a as $myProp, $b as $myProp) {};
?>
--EXPECTF--
Fatal error: Redefinition of captured property $myProp in %s on line %d
