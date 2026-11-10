--TEST--
DCE must not remove ARRAY_KEY_EXISTS with a null key
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php
set_error_handler(function ($errno, $errstr) {
    throw new Exception($errstr);
});

function test(null $k) {
    array_key_exists($k, []);
}

try {
    test(null);
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Using null as the key parameter for array_key_exists() is deprecated, use an empty string instead
