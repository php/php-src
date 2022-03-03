--TEST--
JIT ADD: 001
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
  $res = $var + 1;
  var_dump($res);
}
foo(1);
?>
--EXPECT--
int(2)
