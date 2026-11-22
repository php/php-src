--TEST--
JIT ASSIGN_DIM: undefined dim is reported before undefined OP_DATA when the container is false
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--FILE--
<?php
function test() {
    $x = false;
    $x[$undefKey] = $undef;
    var_dump($x);
}
test();
?>
--EXPECTF--
Deprecated: Automatic conversion of false to array is deprecated in %s on line %d

Warning: Undefined variable $undefKey in %s on line %d

Warning: Undefined variable $undef in %s on line %d
array(1) {
  [""]=>
  NULL
}
