--TEST--
JIT UCALL: 002
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
function foo() {
    var_dump("hello");
}
foo();
var_dump("world!");
?>
--EXPECT--
string(5) "hello"
string(6) "world!"
