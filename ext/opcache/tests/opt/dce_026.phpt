--TEST--
DCE must not remove ISSET_ISEMPTY_DIM_OBJ with a null key
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
    $a = [];
    isset($a[$k]);
}

try {
    test(null);
} catch (Exception $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Using null as an array offset is deprecated, use an empty string instead
