--TEST--
DCE must not remove ARRAY_KEY_EXISTS with a float key
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

function test(float $k) {
    array_key_exists($k, []);
}

try {
    test(1.5);
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Implicit conversion from float 1.5 to int loses precision
