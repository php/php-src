--TEST--
JIT FETCH_DIM_IS: leading-numeric string offset conversion may throw
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php
set_error_handler(function ($errno, $errstr) {
    throw new Exception($errstr);
});

function make(int $n) {
    return str_repeat("abc", $n);
}

function consume($v) {
    echo "unreachable\n";
}

function test(string $s) {
    consume($s["1x"] ?? "default");
}

try {
    test(make(2));
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Illegal string offset "1x"
