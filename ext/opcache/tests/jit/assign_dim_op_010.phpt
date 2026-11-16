--TEST--
JIT ASSIGN_DIM_OP: CONCAT of a temporary string into a typed reference
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--FILE--
<?php
class C {
    public ?string $p = null;
}
function f($n) {
    $c = new C;
    $arr = [5 => null];
    $arr[5] = &$c->p;
    $arr[5] .= dechex($n);
    return $c;
}
var_dump(f(255));
?>
--EXPECT--
object(C)#1 (1) {
  ["p"]=>
  string(2) "ff"
}
