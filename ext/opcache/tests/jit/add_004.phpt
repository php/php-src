--TEST--
JIT ADD: 004
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
function foo($var) {
  $ret = $var + 200;
  var_dump($ret);
}
foo(PHP_INT_MAX);
?>
--EXPECT--
float(9.223372036854776E+18)
