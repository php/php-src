--TEST--
Bug #44827 (define() allows :: in constant names)
--FILE--
<?php

try {
    define('foo::bar', 1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    define('::', 1);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
define(): Argument #1 ($constant_name) cannot be a class constant
define(): Argument #1 ($constant_name) cannot be a class constant
