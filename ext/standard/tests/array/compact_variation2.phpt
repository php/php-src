--TEST--
Test compact() function: ensure compact() doesn't pick up variables declared outside of current scope.
--FILE--
<?php
/* Prototype  : proto array compact(mixed var_names [, mixed ...])
* Description: Creates a hash containing variables and their values
* Source code: ext/standard/array.c
* Alias to functions:
*/
echo "*** Testing compact() : usage variations  - variables outside of current scope ***\n";

$a = 'main.a';
$b = 'main.b';

function f() {
	$b = 'f.b';
	$c = 'f.c';
	var_dump(compact('a','b','c'));
	var_dump(compact(array('a','b','c')));
}

f();

?>
==Done==
--EXPECTF--
*** Testing compact() : usage variations  - variables outside of current scope ***
array(2) {
  ["b"]=>
  string(3) "f.b"
  ["c"]=>
  string(3) "f.c"
}
array(2) {
  ["b"]=>
  string(3) "f.b"
  ["c"]=>
  string(3) "f.c"
}
==Done==