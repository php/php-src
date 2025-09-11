--TEST--
__wakeup() deprecation promoted to exception
--FILE--
<?php

set_error_handler(function ($errno, $errstr, $errfile, $errline) {
    throw new ErrorException($errstr);
});

try {
    require __DIR__ . '/wakeup_deprecation_promoted_exception2.inc';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ErrorException: The __wakeup() serialization magic method has been deprecated. Implement __unserialize() instead (or in addition, if support for old PHP versions is necessary)