--TEST--
Handling of errors during linking
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_inheritance_error_ind.inc
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
echo "Foobar\n";
?>
--EXPECT--
Fatal error: Declaration of B::foo($bar) must be compatible with A::foo() in Unknown on line 0
