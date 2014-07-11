--TEST--
Bug #66176 (Invalid constant substitution)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo($v) {
	global $a;
	return $a[$v];
}
$a = array(PHP_VERSION => 1);
var_dump(foo(PHP_VERSION));
--EXPECT--
int(1)
