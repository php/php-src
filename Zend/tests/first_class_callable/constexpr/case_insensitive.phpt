--TEST--
Allow defining FCC in const expressions with case-insensitive names.
--FILE--
<?php

const Closure = StrRev(...);

var_dump(Closure);
var_dump((Closure)("abc"));

?>
--EXPECTF--
object(Closure)#%d (2) {
  ["function"]=>
  string(%d) "%s"
  ["parameter"]=>
  array(1) {
    ["$string"]=>
    string(10) "<required>"
  }
}
string(3) "cba"
