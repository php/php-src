--TEST--
JIT: compound string key assignment turns a packed array into a hash with integer keys
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=function
--EXTENSIONS--
opcache
--FILE--
<?php
function f() {
    $arr = [1, 2, 3];
    $arr["a"] += 1;
    var_dump($arr[1]);
}
f();
?>
--EXPECTF--
Warning: Undefined array key "a" in %s on line %d
int(2)
