--TEST--
Block pass must not remove the source of the ZEND_FAST_CALL OP2 return value
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=disable
--ENV--
USE_ZEND_ALLOC=0
USE_TRACKED_ALLOC=1
--FILE--
<?php
function test_scalar() {
    $x = 1;
    try {
        try {
            return true ? 1.5 : 2.5;
        } finally {
            undef_fn();
        }
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    return "fallback";
}

function test_refcounted() {
    $x = 1;
    try {
        try {
            return true ? "returned" : "other";
        } finally {
            undef_fn();
        }
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    return "fallback";
}

var_dump(test_scalar());
var_dump(test_refcounted());
?>
--EXPECT--
Error: Call to undefined function undef_fn()
string(8) "fallback"
Error: Call to undefined function undef_fn()
string(8) "fallback"
