--TEST--
Bug GH-16368: gmp_init on first argument as null.
--EXTENSIONS--
gmp
--FILE--
<?php
gmp_init($inexistent);
?>
--EXPECTF--
Warning: Undefined variable $inexistent in %s on line %d

Deprecated: gmp_init(): Passing null to parameter #1 ($num) of type string|int is deprecated in %s on line %d
