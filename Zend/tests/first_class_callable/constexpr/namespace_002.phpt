--TEST--
Allow defining FCC in const expressions in a namespace with global function fallback.
--FILE--
<?php

namespace Foo;

const Closure = strrev(...);

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
