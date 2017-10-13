--TEST--
Bug #75370 (Webserver hangs on valid PHP text)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function test()
{
	$success = true;
	$success = $success AND true;
	return $success;
}

var_dump(test());
?>
--EXPECT--
bool(true)
