--TEST--
JIT RECV: 005
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
  var_dump($var);
}
foo(1);
foo(1.0);
foo("hello");
foo(array());
?>
--EXPECT--
int(1)
float(1)
string(5) "hello"
array(0) {
}
