--TEST--
Bug #66474 (Optimizer bug in constant string to boolean conversion)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo() {
	$speed = 'slow' || 'fast';
}
foo();
echo "ok\n";
--EXPECT--
ok
