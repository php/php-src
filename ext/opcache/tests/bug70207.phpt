--TEST--
Bug #70207 Finally is broken with opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function bar() {
	return "bar";
}
function foo() {
    try { return bar(); }
    finally { @fclose(null); }
}

var_dump(foo());
?>
--EXPECT--
string(3) "bar"
