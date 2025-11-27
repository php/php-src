--TEST--
Broken live-range calculation with QM_ASSIGN optimization
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

function test($cond) {
    $caller = $cond ? new \ReflectionMethod('non', 'existent') : null;
}

try {
    test(true);
} catch (Throwable $e) {
    echo $e::class, $e->getMessage(), "\n";
}

?>
--EXPECT--
ReflectionExceptionClass "non" does not exist
