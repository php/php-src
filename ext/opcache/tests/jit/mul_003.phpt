--TEST--
JIT MUL: 003 boundary value for optmizing MUL to SHIFT
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=32M
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php

function mul2_bound(int $a) {
  $res = $a * -2147483648;
  var_dump($res);
}

function mul1_bound(int $a) {
  $res = -2147483648 * $a;
  var_dump($res);
}

mul2_bound(3);
mul1_bound(3);
?>
--EXPECT--
int(-6442450944)
int(-6442450944)