--TEST--
__sleep() deprecation promoted to exception
--FILE--
<?php

set_error_handler(function ($errno, $errstr, $errfile, $errline) {
    throw new ErrorException($errstr);
});

try {
eval(<<<'CODE'
    class Test {
        public function __sleep() {
            return [];
        }
    }
CODE);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ErrorException: The __sleep() serialization magic method has been deprecated. Implement __serialize() instead (or in addition, if support for old PHP versions is necessary)
