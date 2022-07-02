--TEST--
JIT ADD: 006
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=32M
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo($a, $b) {
  $res = $a + $b;
  var_dump($res);
}
foo(3, 5);
foo(3.0, 5.0);
foo(3.0, 5);
foo(3, 5.0);
?>
--EXPECT--
int(8)
float(8)
float(8)
float(8)
