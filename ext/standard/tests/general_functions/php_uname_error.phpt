--TEST--
php_uname(): Invalid arguments
--FILE--
<?php

try {
    var_dump(php_uname(''));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(php_uname('test'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(php_uname('z'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: php_uname(): Argument #1 ($mode) must be a single character
ValueError: php_uname(): Argument #1 ($mode) must be a single character
ValueError: php_uname(): Argument #1 ($mode) must be one of "a", "m", "n", "r", "s", or "v"
