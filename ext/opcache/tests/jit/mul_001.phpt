--TEST--
JIT MUL: 001 integer multiplay
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
function mul2(int $a) {
  $res = $a * 2;
  var_dump($res);
}
function mul4(int $a) {
  $res = $a * 4;
  var_dump($res);
}
function mul111(int $a) {
  $res = $a * 111;
  var_dump($res);
}
mul2(3);
mul4(3);
mul111(3);
?>
--EXPECT--
int(6)
int(12)
int(333)
