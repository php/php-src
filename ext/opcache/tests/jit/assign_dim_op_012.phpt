--TEST--
JIT ASSIGN_DIM_OP: ADD of a temporary array
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--FILE--
<?php
function f($n) {
    $arr = [5 => []];
    $arr[5] += $n ? [1, str_repeat("a", $n)] : 1;
    return $arr;
}
var_dump(f(3));
?>
--EXPECT--
array(1) {
  [5]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    string(3) "aaa"
  }
}
