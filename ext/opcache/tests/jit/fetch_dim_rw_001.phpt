--TEST--
JIT FETCH_DIM_RW: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
;opcache.jit_debug=257
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo() {
    $a[0][0] += 2;
    return $a[0];
}
var_dump(foo());
?>
--EXPECTF--
Warning: Undefined variable $a in %s on line %d

Warning: Undefined array key 0 in %sfetch_dim_rw_001.php on line 3

Warning: Undefined array key 0 in %sfetch_dim_rw_001.php on line 3
array(1) {
  [0]=>
  int(2)
}
