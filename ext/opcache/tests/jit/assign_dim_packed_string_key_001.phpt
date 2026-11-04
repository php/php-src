--TEST--
JIT: string key write turns a packed array into a hash with integer keys (function JIT)
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
    $arr = [10, 11];
    for ($i = 2; $i >= 0; $i--) {
        $arr['b'] = true;
        echo $i, ":\n";
        var_dump($arr[$i]);
    }
}
f();
?>
--EXPECTF--
2:

Warning: Undefined array key 2 in %s on line %d
NULL
1:
int(11)
0:
int(10)
