--TEST--
JIT ICALL: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
var_dump(true, 0, 42, -42, 0.0, 2.0,"hello", array());
?>
--EXPECT--
bool(true)
int(0)
int(42)
int(-42)
float(0)
float(2)
string(5) "hello"
array(0) {
}
