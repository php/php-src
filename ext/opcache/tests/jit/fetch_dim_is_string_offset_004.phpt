--TEST--
JIT FETCH_DIM_IS: leading-numeric string offset warning may release the string
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
    echo "Warning: $errstr\n";
    $GLOBALS['s'] = null;
});

function make(int $n) {
    return str_repeat("abc", $n);
}

function test() {
    global $s;
    return $s["1x"] ?? "default";
}

$s = make(2);
var_dump(test());
?>
--EXPECT--
Warning: Illegal string offset "1x"
string(7) "default"
