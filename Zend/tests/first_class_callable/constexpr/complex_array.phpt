--TEST--
Allow defining FCC wrapped in an array in const expressions.
--FILE--
<?php

const Closure = [strrev(...), strlen(...)];

var_dump(Closure);

foreach (Closure as $closure) {
    var_dump($closure("abc"));
}

?>
--EXPECTF--
array(2) {
  [0]=>
  object(Closure)#%d (2) {
    ["function"]=>
    string(6) "strrev"
    ["parameter"]=>
    array(1) {
      ["$string"]=>
      string(10) "<required>"
    }
  }
  [1]=>
  object(Closure)#%d (2) {
    ["function"]=>
    string(6) "strlen"
    ["parameter"]=>
    array(1) {
      ["$string"]=>
      string(10) "<required>"
    }
  }
}
string(3) "cba"
int(3)
