--TEST--
GH-25169: OSR compilation does not use a bound spill slot that cannot be initialized at the entry
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1235
opcache.jit_buffer_size=64M
opcache.jit_hot_loop=1
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php
function loop(int $n): float {
    $step = 1 / $n;
    $x = 1.0;
    $sum = 0.0;
    for ($i = 1; $i <= $n; $i++) {
        $prev = $x;
        $x -= $step;
        if ($i === $n) {
            $sum += 1;
        }
        $sum += $prev;
    }
    return $sum;
}

var_dump(loop(64));
var_dump(loop(64));
?>
--EXPECT--
float(33.5)
float(33.5)
