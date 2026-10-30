--TEST--
JIT INC: 022
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function inc($x) {
    return ++$x;
}
function dec($x) {
    return --$x;
}
var_dump(inc("abc"));
var_dump(inc("5"));
var_dump(inc(1.1));
var_dump(dec("5"));
var_dump(dec(1.1));
?>
--EXPECT--
string(3) "abd"
int(6)
float(2.1)
int(4)
float(0.10000000000000009)
