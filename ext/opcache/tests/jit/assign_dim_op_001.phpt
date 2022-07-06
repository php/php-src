--TEST--
JIT ASSIGN_DIM_OP: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$x = "a";
$a[$x . "b"] = 0;
$a[$x . "b"] += 2;
var_dump($a);
?>
--EXPECT--
array(1) {
  ["ab"]=>
  int(2)
}
