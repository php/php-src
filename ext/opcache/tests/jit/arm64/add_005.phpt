--TEST--
JIT ADD: 005
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
  $res = $var + 2;
  var_dump($res);
}
foo("hello");
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Unsupported operand types: string + int in %s:%d
Stack trace:
#0 %s(%d): foo('hello')
#1 {main}
  thrown in %s on line %d
