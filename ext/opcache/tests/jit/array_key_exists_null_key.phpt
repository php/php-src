--TEST--
JIT ARRAY_KEY_EXISTS: null key conversion may throw
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

function test(null $k) {
    var_dump(array_key_exists($k, []));
}

try {
    test(null);
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Using null as the key parameter for array_key_exists() is deprecated, use an empty string instead
