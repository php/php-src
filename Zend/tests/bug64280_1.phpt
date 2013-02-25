--TEST--
Bug #64280 (__destruct loop segfaults) - tests ini entry "max_magic_calls" and combining magic methods/functions with functions
--INI--
max_magic_calls = 20
--FILE--
<?php
ini_set("max_magic_calls", 30);
ini_set("max_magic_calls", 10);
class getStringed {
	public function __toString() {
		global $count;
		echo ++$count,"\n";
		f();
	}
}
function f() {
	$class = new getStringed;
	print $class;
}
f();
--EXPECTF--
Warning: max_implicit_function_calls ini-value has to be equal or less than php.ini value and greater than 1 in %s on line %d
1
2
3
4
5
6
7
8
9
10

Fatal error: Method or function too often called recursively by implicit call in %s on line %d
