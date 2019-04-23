--TEST--
Keep BIND_STATIC when initial value refer to unresolved constants
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo() {
	static $a = UNDEFINED_CONST;
}
foo();
?>
OK
--EXPECTF--
Warning: Use of undefined constant UNDEFINED_CONST - assumed 'UNDEFINED_CONST' (this will throw an Error in a future version of PHP) in %s on line %d
OK