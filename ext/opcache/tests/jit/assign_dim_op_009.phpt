--TEST--
JIT ASSIGN_DIM_OP: CONCAT of a temporary string into a referenced null element
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--FILE--
<?php
function f($n) {
    $arr = [5 => null];
    $r = &$arr[5];
    $arr[5] .= dechex($n);
    return $r;
}
var_dump(f(255));
?>
--EXPECT--
string(2) "ff"
