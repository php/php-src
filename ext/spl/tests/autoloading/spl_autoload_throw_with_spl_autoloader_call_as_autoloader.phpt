--TEST--
spl_autoload_register() function - warn when using spl_autoload_call() as the autoloading function
--FILE--
<?php

try {
    spl_autoload_register('spl_autoload_call');
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: spl_autoload_register(): Argument #1 ($callback) must not be the spl_autoload_call() function
