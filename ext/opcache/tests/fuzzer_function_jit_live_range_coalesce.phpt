--TEST--
Live range of ZEND_COALESCE must not cover the fall-through path
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
$s = "b";
try {
    if ("a" . $s) {
        $item ??= match (true) { 1 => 1 };
    }
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

function coalesce($a, $b) {
    return $a ?? $b;
}
var_dump(coalesce("x", "y"), coalesce(null, "y"));
echo "OK\n";
?>
--EXPECT--
UnhandledMatchError: Unhandled match case true
string(1) "x"
string(1) "y"
OK
