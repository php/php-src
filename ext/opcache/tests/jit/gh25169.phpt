--TEST--
GH-25169: JIT keeps the old value of a reassigned CV live across an internal call
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=64M
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php
function float_step(float $x, int|float $step): float {
    $prev = $x;
    $x -= $step;
    sin(0.0);
    return $prev;
}

function int_step(float $x, int|float $step): float {
    $prev = $x;
    $x -= $step;
    sin(0.0);
    return $prev;
}

function int_value(int $x, int|float $step): int {
    $prev = $x;
    $x -= $step;
    sin(0.0);
    return $prev;
}

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

var_dump(float_step(1.0, 0.25));
var_dump(int_step(1.0, 1));
var_dump(int_value(1, 1));
var_dump(loop(64));
?>
--EXPECT--
float(1)
float(1)
int(1)
float(33.5)
