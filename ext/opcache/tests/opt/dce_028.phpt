--TEST--
DCE must not remove FETCH_DIM_IS of a nested isset()/empty() with a leading-numeric string offset
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

function test_isset(string $s) {
    $x = isset($s["1x"][0]);
}

function test_empty(string $s) {
    $x = empty($s["1x"][0]);
}

foreach (['test_isset', 'test_empty'] as $f) {
    try {
        $f("abc");
    } catch (Exception $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
Exception: Illegal string offset "1x"
Exception: Illegal string offset "1x"
