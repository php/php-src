--TEST--
GH-23693: Tracing JIT reads stale flags for a guard on a hoisted addition
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=tracing
opcache.jit_hot_func=1
--EXTENSIONS--
opcache
--FILE--
<?php
function f(int $pos, int $n): int {
    if ($pos < 0) {
        return -1;
    }
    $y = $pos >> 2;
    $s = 0;
    for ($dy = -1; $dy <= 1; ++$dy) {
        for ($i = 0; $i < $n; ++$i) {
            $ny = $y + $dy;
            if ($ny < 0) {
                continue;
            }
            if ($ny >= 4) {
                continue;
            }
            $s += ($ny << 2) | ($i & 3);
        }
    }
    return $s;
}
for ($k = 0; $k < 30; ++$k) {
    f(16, 200);
}
var_dump(f(16, 200));
var_dump(f(0, 200));
?>
--EXPECT--
int(2700)
int(1400)
