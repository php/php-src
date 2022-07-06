--TEST--
JIT CAST: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.jit=1205
;opcache.jit_debug=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo (int $x = null) {
    $a = (array)$x;
    $a[] = 42;
    var_dump($a);
}
foo(null);
?>
--EXPECT--
array(1) {
  [0]=>
  int(42)
}
