--TEST--
Bug #68252 (segfault in Zend/zend_hash.c in function _zend_hash_del_el)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.fast_shutdown=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
/* run this test script with valgrind */
function a() {
	echo "okey";
}

create_function('', 'var_dump("22");');

a();
--EXPECTF--
Deprecated: Function create_function() is deprecated in %s on line %d
okey
