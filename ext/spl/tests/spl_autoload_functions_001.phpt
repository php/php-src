--TEST--
SPL autoloader can autoload functions
--FILE--
<?php

spl_autoload_register(function(string $name) {
    echo "name=$name\n";
}, true, false, SPL_AUTOLOAD_FUNCTION);

spl_autoload_call('foo', SPL_AUTOLOAD_FUNCTION);
?>
--EXPECT--
name=foo
