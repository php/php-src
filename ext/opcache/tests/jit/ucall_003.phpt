--TEST--
JIT UCALL: 003
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
  var_dump("hello");
}
function bar() {
  foo();
}
bar();
?>
--EXPECT--
string(5) "hello"
