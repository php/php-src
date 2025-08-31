--TEST--
Allow defining FCC in class constants.
--FILE--
<?php

class C {
    const Closure = strrev(...);
}

var_dump(C::Closure);
var_dump((C::Closure)("abc"));

?>
--EXPECTF--
object(Closure)#%d (2) {
  ["function"]=>
  string(6) "strrev"
  ["parameter"]=>
  array(1) {
    ["$string"]=>
    string(10) "<required>"
  }
}
string(3) "cba"
