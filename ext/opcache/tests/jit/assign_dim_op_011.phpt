--TEST--
JIT ASSIGN_DIM_OP: CONCAT of a temporary string into an ArrayAccess offset
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--FILE--
<?php
class A implements ArrayAccess {
    public $d = [];
    function offsetGet($o): mixed { return $this->d[$o] ?? null; }
    function offsetSet($o, $v): void { $this->d[$o] = $v; }
    function offsetExists($o): bool { return isset($this->d[$o]); }
    function offsetUnset($o): void {}
}
function f($n) {
    $a = new A;
    $a[5] .= dechex($n);
    return $a->d;
}
var_dump(f(255));
?>
--EXPECT--
array(1) {
  [5]=>
  string(2) "ff"
}
