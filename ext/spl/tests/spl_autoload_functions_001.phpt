--TEST--
SPL autoloader can autoload functions
--FILE--
<?php

spl_autoload_register(function(string $name, int $mode) {
    echo "name=$name, mode=$mode\n";
}, true, false, SPL_AUTOLOAD_FUNCTION);

spl_autoload_call('foo', SPL_AUTOLOAD_FUNCTION);
?>
--EXPECT--
name=foo, mode=2
