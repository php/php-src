--TEST--
spl_autoload cannot be used to autoload functions
--FILE--
<?php

spl_autoload_register(function(string $name, int $mode) {
    echo "name=$name, mode=$mode\n";
}, true, false, SPL_AUTOLOAD_FUNCTION);

spl_autoload_call('foo', SPL_AUTOLOAD_FUNCTION);
spl_autoload('foo', SPL_AUTOLOAD_FUNCTION);
?>
--EXPECTF--
name=foo, mode=2

Fatal error: Uncaught Error: Default autoloader can only load classes. in %s:%d
Stack trace:
#0 %s(%d): spl_autoload('foo', 2)
#1 {main}
  thrown in %s on line %d
