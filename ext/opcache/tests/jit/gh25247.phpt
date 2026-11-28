--TEST--
GH-25247 (Tracing JIT: argument 13+ keeps a stale value when the operand is a QM_ASSIGN left in a register)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=64M
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
--FILE--
<?php
function helper_int() { return 1; }
function helper_str() { return 's'; }

$f = function ($a1, $a2, $a3, $a4, $a5, $a6, $a7, $a8, $a9, $a10, $a11, $a12, $a13) {
    return $a13;
};
$f12 = function ($a1, $a2, $a3, $a4, $a5, $a6, $a7, $a8, $a9, $a10, $a11, $a12, $a13) {
    return $a12;
};

$o = new stdClass;
$o->x = 'x';
$o->y = 1;
$c = true;
$x = null;
$a = 1;
$b = 2;

for ($i = 0; $i < 5; $i++) {
    $checks = [
        [$f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, $o->x, $o->y === 1 ? 1 : 2), 1],
        [$f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, $c ? helper_int() : 0), 1],
        [$f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, $c ? max($a, $b) : 0), 2],
        [$f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, $x ?? helper_int()), 1],
        [$f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, $o->y === 1 ? 1.5 : 2.5), 1.5],
        [$f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, $c ? helper_str() : 'no'), 's'],
        [$f12(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, $o->y === 1 ? 1 : 2, 13), 1],
    ];
    foreach ($checks as $n => [$got, $expect]) {
        if ($got !== $expect) {
            echo "iteration $i check $n: ", var_export($got, true), ", expected ", var_export($expect, true), "\n";
            exit(1);
        }
    }
}
echo "OK\n";
?>
--EXPECT--
OK
