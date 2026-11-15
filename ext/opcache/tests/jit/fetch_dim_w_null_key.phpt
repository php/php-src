--TEST--
JIT FETCH_DIM_W: null key conversion may throw
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

class C {
    function __destruct() {
        echo "destruct\n";
    }
}

function g($x, &$y) {}

function test(null $k) {
    $c = new C;
    g($c, $a[$k]);
}

try {
    test(null);
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
destruct
Exception: Using null as an array offset is deprecated, use an empty string instead
