--TEST--
JIT MUL: 002 integer overflow
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
function mul(int $a, int $b) {
  $res = $a * $b;
  var_dump($res);
}
mul(0x5555555555, 0x5555555555);
?>
--EXPECT--
float(1.343250910680478E+23)
