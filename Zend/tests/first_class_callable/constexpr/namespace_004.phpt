--TEST--
Allow defining FCC in const expressions in a namespace with function matching a global function later.
--FILE--
<?php

namespace Foo;

function foo(\Closure $c = strrev(...)) {
    $d = strrev(...);
    var_dump($c);
    var_dump($c("abc"));
    var_dump($d);
    var_dump($d("abc"));
}


foo();

if (random_int(1, 2) > 0) {
    function strrev(string $value) {
        return 'not the global one';
    }
}

foo();

?>
--EXPECTF--
object(Closure)#1 (2) {
  ["function"]=>
  string(6) "strrev"
  ["parameter"]=>
  array(1) {
    ["$string"]=>
    string(10) "<required>"
  }
}
string(3) "cba"
object(Closure)#2 (2) {
  ["function"]=>
  string(6) "strrev"
  ["parameter"]=>
  array(1) {
    ["$string"]=>
    string(10) "<required>"
  }
}
string(3) "cba"
object(Closure)#2 (2) {
  ["function"]=>
  string(6) "strrev"
  ["parameter"]=>
  array(1) {
    ["$string"]=>
    string(10) "<required>"
  }
}
string(3) "cba"
object(Closure)#1 (2) {
  ["function"]=>
  string(6) "strrev"
  ["parameter"]=>
  array(1) {
    ["$string"]=>
    string(10) "<required>"
  }
}
string(3) "cba"
