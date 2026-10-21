--TEST--
JIT ASSIGN_DIM_OP: CONCAT of a temporary string into an empty element
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--FILE--
<?php
function f($n) {
    $arr = [5 => null];
    $arr[5] .= dechex($n);
    $x = str_repeat("z", 2);
    return $arr;
}
var_dump(f(255));
var_dump(f(256));
?>
--EXPECT--
array(1) {
  [5]=>
  string(2) "ff"
}
array(1) {
  [5]=>
  string(3) "100"
}
