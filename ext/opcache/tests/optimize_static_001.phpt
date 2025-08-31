--TEST--
Keep BIND_STATIC when initial value refer to unresolved constants
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    static $a = UNDEFINED_CONST;
}
try {
    foo();
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}
?>
OK
--EXPECT--
Exception: Undefined constant "UNDEFINED_CONST"
OK
