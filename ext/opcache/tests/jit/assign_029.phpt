--TEST--
JIT ASSIGN: 029
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
  $i = 1;
  var_dump($i=2);
  return $i;
}
var_dump(foo());
?>
--EXPECT--
int(2)
int(2)
