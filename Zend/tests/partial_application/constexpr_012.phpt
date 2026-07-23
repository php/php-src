--TEST--
PFA in constexpr: nested const expr
--FILE--
<?php

function f($a = g(new stdClass, g(...), ...)) {
    return $a(1);
}

function g($a, $b, $c) {
    return [$a, $b, $c];
}

var_dump(f());

?>
--EXPECTF--
array(3) {
  [0]=>
  object(stdClass)#%d (0) {
  }
  [1]=>
  object(Closure)#%d (2) {
    ["function"]=>
    string(1) "g"
    ["parameter"]=>
    array(3) {
      ["$a"]=>
      string(10) "<required>"
      ["$b"]=>
      string(10) "<required>"
      ["$c"]=>
      string(10) "<required>"
    }
  }
  [2]=>
  int(1)
}
