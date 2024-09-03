--TEST--
autoloading different cases results in calling the autoloader once
--FILE--
<?php

spl_autoload_register(function(string $name) {
    echo "name=$name\n";
    eval("function $name() { echo \"$name\"; }");
}, true, false, SPL_AUTOLOAD_FUNCTION);

FOO();
foo();
?>
--EXPECT--
name=FOO
FOOFOO
