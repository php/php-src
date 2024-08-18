--TEST--
autoloading different cases results in calling the autoloader once
--FILE--
<?php

spl_autoload_register(function(string $name, int $mode) {
    echo "name=$name, mode=$mode\n";
    eval("function $name() { echo \"$name\"; }");
}, true, false, SPL_AUTOLOAD_FUNCTION);

FOO();
foo();
?>
--EXPECT--
name=FOO, mode=2
FOOFOO
