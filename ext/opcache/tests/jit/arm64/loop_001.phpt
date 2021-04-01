--TEST--
JIT LOOP: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=32M
;opcache.jit_debug=257
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo($n) {
  $res = 0;
  for ($i = 1; $i < $n; $i++)
    $res = $res + $i;
  return $res;
}
print foo(5);
?>
--EXPECT--
10
