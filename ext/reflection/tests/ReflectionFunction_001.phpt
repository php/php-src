--TEST--
ReflectionFunction methods
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
	var_dump($funcInfo->getName());
	var_dump($funcInfo->isInternal());
	var_dump($funcInfo->isUserDefined());
	var_dump($funcInfo->getStartLine());
	var_dump($funcInfo->getEndLine());
	var_dump($funcInfo->getStaticVariables());
}

dumpFuncInfo('foo');
dumpFuncInfo('bar');
dumpFuncInfo('extract');

?>
--EXPECT--
string(3) "foo"
bool(false)
bool(true)
int(6)
int(11)
array(3) {
  ["c"]=>
  NULL
  ["a"]=>
  int(1)
  ["b"]=>
  string(5) "hello"
}
string(3) "bar"
bool(false)
bool(true)
int(16)
int(16)
array(0) {
}
string(7) "extract"
bool(true)
bool(false)
bool(false)
bool(false)
array(0) {
}
