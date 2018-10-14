--TEST--
ReflectionFunction::getDocComment()
--CREDITS--
Robin Fernandes <robinf@php.net>
Steve Seear <stevseea@php.net>
--INI--
opcache.save_comments=1
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
	var_dump($funcInfo->getDocComment());
}

dumpFuncInfo('foo');
dumpFuncInfo('bar');
dumpFuncInfo('extract');

?>
--EXPECTF--
string(%d) "/**
 * my doc comment
 */"
bool(false)
bool(false)
