--TEST--
JIT: unpacking a hash array with integer keys produces a packed array
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=1205
--EXTENSIONS--
opcache
--FILE--
<?php
function f($c) {
    $b = $c ? [-5 => 1, -6 => 2, -7 => 3] : [-7 => 2];
    $a = [...$b];
    foreach ($a as $k => $v) {
        echo "$k => $v\n";
    }
}
f(true);
?>
--EXPECT--
0 => 1
1 => 2
2 => 3
