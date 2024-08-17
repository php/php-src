--TEST--
Parse errors should be thrown if occuring from an autoloader
--FILE--
<?php

autoload_register_class(function($class) {
    eval("ha ha ha");
});
autoload_register_class(function($class) {
    echo "Don't call me.\n";
});

new Foo;

?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "ha" in %s on line %d
