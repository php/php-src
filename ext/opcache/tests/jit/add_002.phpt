--TEST--
JIT ADD: 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo($var) {
  $res = $var + 0x1000;
  var_dump($res);
}
foo(1);
?>
--EXPECT--
int(4097)
