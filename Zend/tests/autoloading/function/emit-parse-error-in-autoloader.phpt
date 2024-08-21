--TEST--
Parse errors should be thrown if occuring from an autoloader
--XFAIL--
Problem with JIT that needs to be resolved
--FILE--
<?php

autoload_register_function(function($class) {
    eval("ha ha ha");
});
autoload_register_function(function($class) {
    echo "Don't call me.\n";
});

foo();

?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "ha" in %s on line %d
