--TEST--
Bug #78429 (opcache_compile_file(__FILE__); segfaults)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--INI--
opcache.enable_cli=0
--FILE--
<?php
var_dump(opcache_compile_file(__FILE__));
?>
--EXPECTF--
Notice: Zend OPcache has not been properly started, can't compile file in %s on line %d
bool(false)
