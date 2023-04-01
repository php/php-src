--TEST--
Cannot use property capture and explicitly pass parameters to constructor
--FILE--
<?php
$a = 42;
$foo = new class(101) use ($a) {
};
?>
--EXPECTF--
Fatal error: Cannot pass constructor arguments to anonymous class with captured properties in %s on line %d
