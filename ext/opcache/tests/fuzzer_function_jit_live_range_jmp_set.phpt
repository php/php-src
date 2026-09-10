--TEST--
Live range of ZEND_JMP_SET must not cover the fall-through path
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
try {
    $a ?: match (true) { 1 => 1 };
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

function elvis($a, $b) {
    return $a ?: $b;
}
var_dump(elvis("x", "y"), elvis("", "y"), elvis(null, "z"));
echo "OK\n";
?>
--EXPECTF--
Warning: Undefined variable $a in %s on line %d
UnhandledMatchError: Unhandled match case true
string(1) "x"
string(1) "y"
string(1) "z"
OK
