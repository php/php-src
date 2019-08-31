--TEST--
ReflectionFunction::getFileName()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--FILE--
<?php

/**
 * my doc comment
 */
function foo () {
	static $c;
	static $a = 1;
	static $b = "hello";
	$d = 5;
}

/***
 * not a doc comment
 */
function bar () {}


function dumpFuncInfo($name) {
	$funcInfo = new ReflectionFunction($name);
	var_dump($funcInfo->getFileName());
}

dumpFuncInfo('foo');
dumpFuncInfo('bar');
dumpFuncInfo('extract');

?>
--EXPECTF--
string(%d) "%sReflectionFunction_getFileName.002.php"
string(%d) "%sReflectionFunction_getFileName.002.php"
bool(false)
