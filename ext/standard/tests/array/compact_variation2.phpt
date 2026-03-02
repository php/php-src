--TEST--
Test compact() function: ensure compact() doesn't pick up variables declared outside of current scope.
--FILE--
<?php
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
--EXPECTF--
*** Testing compact() : usage variations  - variables outside of current scope ***

Warning: compact(): Undefined variable $a in %s on line %d
array(2) {
  ["b"]=>
  string(3) "f.b"
  ["c"]=>
  string(3) "f.c"
}

Warning: compact(): Undefined variable $a in %s on line %d
array(2) {
  ["b"]=>
  string(3) "f.b"
  ["c"]=>
  string(3) "f.c"
}
