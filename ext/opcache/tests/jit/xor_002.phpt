--TEST--
JIT XOR: 002
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
function foo($var) {
  $res = $var ^ $var;
  var_dump($res);
}
foo(5);
?>
--EXPECT--
int(0)
