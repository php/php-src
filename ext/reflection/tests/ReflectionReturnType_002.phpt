--TEST--
ReflectionReturnType - internal functions
--SKIPIF--
<?php
if (!function_exists("zend_test_func")) echo "skip";
?>
--FILE--
<?php


function dump_rt_obj($rt) {
	var_dump($rt->isArray());
	var_dump($rt->isCallable());
	var_dump($rt->allowsNull());
	var_dump($rt->isReturnByReference());
	var_dump($rt->getClass());

	echo "=========\n";
}

$rf = new ReflectionFunction('zend_test_func');
dump_rt_obj($rf->getReturnType());

$rf = new ReflectionFunction('zend_test_func2');
dump_rt_obj($rf->getReturnType());

?>
--EXPECTF--
bool(true)
bool(false)
bool(false)
bool(false)
NULL
=========
bool(true)
bool(false)
bool(true)
bool(false)
NULL
=========
