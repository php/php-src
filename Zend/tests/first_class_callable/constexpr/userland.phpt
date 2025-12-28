--TEST--
Allow defining FCC for userland functions in const expressions.
--FILE--
<?php

function my_function(string $foo) {
    echo "Called ", __FUNCTION__, PHP_EOL;
    var_dump($foo);
}

const Closure = my_function(...);

var_dump(Closure);
(Closure)("abc");

?>
--EXPECTF--
object(Closure)#%d (2) {
  ["function"]=>
  string(11) "my_function"
  ["parameter"]=>
  array(1) {
    ["$foo"]=>
    string(10) "<required>"
  }
}
Called my_function
string(3) "abc"
