--TEST--
Throw when using autoload_register_class() as the autoloading function
--FILE--
<?php

try {
    autoload_register_class('autoload_call_class');
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
autoload_register_class(): Argument #1 ($callback) must not be the autoload_call_class() function
