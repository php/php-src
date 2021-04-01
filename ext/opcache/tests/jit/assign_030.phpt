--TEST--
JIT ASSIGN: 030
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
  $i = 1;
  $x = 2;
  var_dump($i=$x);
  return $i;
}
var_dump(foo());
?>
--EXPECT--
int(2)
int(2)
