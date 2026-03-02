--TEST--
Allow defining FCC in const expressions in a namespace with function matching a global function.
--FILE--
<?php

namespace Foo;

function strrev(string $value) {
    return 'not the global one';
}

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
    ["$value"]=>
    string(10) "<required>"
  }
}
string(18) "not the global one"
