--TEST--
JIT SEND_VAL: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--EXTENSIONS--
opcache
--FILE--
<?php
function foo($type) {
        $key = md5(
            is_array($type) ? \implode('_', $type) : $type .
            "ops"
        );
        return $key;
}
var_dump(foo("int"));
var_dump(foo(["int"]));
?>
--EXPECT--
string(32) "253a948ecc9192cb47e492f692aa63a8"
string(32) "fa7153f7ed1cb6c0fcf2ffb2fac21748"
