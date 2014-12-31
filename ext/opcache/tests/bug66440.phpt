--TEST--
Bug #66440 (Optimisation of conditional JMPs based on pre-evaluate constant function calls)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
if(constant('PHP_BINARY')) {
	echo "OK\n";
}
--EXPECT--
OK
