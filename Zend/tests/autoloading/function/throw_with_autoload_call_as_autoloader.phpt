--TEST--
Throw when using autoload_register_function() as the autoloading function
--FILE--
<?php

try {
    autoload_register_function('autoload_call_function');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
autoload_register_function(): Argument #1 ($callback) must not be the autoload_call_function() function
