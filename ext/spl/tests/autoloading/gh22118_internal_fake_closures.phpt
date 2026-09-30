--TEST--
GH-22118: spl_autoload_unregister() should also unregister fake closure representing internal function
--FILE--
<?php

spl_autoload_register(strlen(...));
var_dump(spl_autoload_functions());
var_dump(spl_autoload_unregister('strlen'));
var_dump(spl_autoload_functions());

?>
--EXPECT--
array(1) {
  [0]=>
  object(Closure)#1 (2) {
    ["function"]=>
    string(6) "strlen"
    ["parameter"]=>
    array(1) {
      ["$string"]=>
      string(10) "<required>"
    }
  }
}
bool(false)
array(1) {
  [0]=>
  object(Closure)#1 (2) {
    ["function"]=>
    string(6) "strlen"
    ["parameter"]=>
    array(1) {
      ["$string"]=>
      string(10) "<required>"
    }
  }
}
