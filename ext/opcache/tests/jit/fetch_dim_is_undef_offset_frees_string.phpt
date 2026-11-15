--TEST--
JIT FETCH_DIM_IS: undefined offset variable when the error handler releases the string
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
    $GLOBALS['s'] = null;
    echo $errstr, "\n";
});

function make(int $n) {
    return str_repeat("abc", $n);
}

function test() {
    global $s;
    return $s[$undef] ?? "default";
}

$s = make(2);
var_dump(test());
?>
--EXPECT--
Undefined variable $undef
string(7) "default"
