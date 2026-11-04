--TEST--
GH-24438 (Tracing JIT preserves indirect arguments on side-trace entry)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=tracing
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
opcache.jit_max_polymorphic_calls=0
--FILE--
<?php
function byVal($value) {
    $value = (int) $value + 1000;
    return $value;
}

function byRef(&$value) {
    $value = (int) $value + 1;
    return $value;
}

$fns = ['byVal', 'byRef'];
$arr = [10, 20];
for ($i = 0; $i < 120; $i++) {
    $f = $fns[$i % 2];
    $f($arr[1]);
    $f($arr[0]);
}

var_dump($arr[0], $arr[1]);
?>
--EXPECT--
int(70)
int(80)
