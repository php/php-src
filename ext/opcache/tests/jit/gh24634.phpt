--TEST--
GH-24634 (Tracing JIT reads a stale stack slot at a side exit)
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
--FILE--
<?php
function f(int $x) {
    $e = $x & 3;
    $m = $x & 4;
    if ($e === 0) {
        if ($m === 0) {
            $r = 0;
        } else {
            $e = 100;
            while ($m < 16) {
                $m <<= 1;
                $e--;
            }
            $r = $e;
        }
    } else {
        $r = $e + 10;
    }
    return $r;
}

$out = [];
for ($i = 0; $i < 8; $i++) {
    $out[] = f($i);
}
echo implode(" ", $out), "\n";
?>
--EXPECT--
0 11 12 13 98 11 12 13
