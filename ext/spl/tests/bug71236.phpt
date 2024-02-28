--TEST--
Bug #71236: Second call of spl_autoload_register() does nothing if it has no arguments
--FILE--
<?php

spl_autoload_register(function ($class) {});
spl_autoload_register();
var_dump(spl_autoload_functions());

?>
--EXPECTF--
array(2) {
  [0]=>
  object(Closure)#%d (2) {
    ["name"]=>
    string(%d) "{closure:%s:%d}"
    ["parameter"]=>
    array(1) {
      ["$class"]=>
      string(10) "<required>"
    }
  }
  [1]=>
  string(12) "spl_autoload"
}
