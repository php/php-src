--TEST--
Bug #76463 (var has array key type but not value type)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

function test() {
	$old_data = isset($old_data) ? (array)$old_data : [];
}

?>
okey
--EXPECT--
okey
