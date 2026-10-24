--TEST--
JIT ASSIGN_DIM: undefined OP_DATA is reported after the false to array deprecation when appending
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--FILE--
<?php
function test() {
    $x = false;
    $x[] = $undef;
    var_dump($x);
}
test();
?>
--EXPECTF--
Deprecated: Automatic conversion of false to array is deprecated in %s on line %d

Warning: Undefined variable $undef in %s on line %d
array(1) {
  [0]=>
  NULL
}
