--TEST--
Bug #74372: autoloading file with syntax error uses next autoloader, may hide parse error
--FILE--
<?php

spl_autoload_register(function($class) {
    eval("ha ha ha");
});
spl_autoload_register(function($class) {
    echo "Don't call me.\n";
});

new Foo;

?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "ha" in %s on line %d
