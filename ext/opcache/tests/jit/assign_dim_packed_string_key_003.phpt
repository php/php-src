--TEST--
JIT: nested string key write turns a packed array into a hash with integer keys (tracing JIT)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=tracing
--EXTENSIONS--
opcache
--FILE--
<?php
function f($c) {
    $arr = [0 => 'x', 5 => 'y'];
    $arr['a'][32] = 1;
    $r = 0;
    for ($i = 0; $i < 100; $i++) {
        $r += isset($arr[0][$c]);
    }
    var_dump($r);
}
for ($k = 0; $k < 3; $k++) {
    f(0);
}
?>
--EXPECT--
int(100)
int(100)
int(100)
