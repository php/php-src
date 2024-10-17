--TEST--
SPL autoloader can unregister function autoloaders
--FILE--
<?php

spl_autoload_register($cb = function(string $name, int $mode) {
    echo "name=$name, mode=$mode\n";
}, true, false, SPL_AUTOLOAD_FUNCTION);

spl_autoload_unregister($cb, SPL_AUTOLOAD_FUNCTION);

bar();
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function bar() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
