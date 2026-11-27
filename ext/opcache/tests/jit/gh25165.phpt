--TEST--
GH-25165 (Tracing JIT: infinite loop once opcache.jit_buffer_size is exhausted while compiling side traces)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=4K
opcache.jit_hot_loop=1
opcache.jit_hot_func=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
--FILE--
<?php
function f0($a) { $x = $a; $i = 0; do { $x = $x + 0; } while (++$i < 4); return $x; }
function f1($a) { $x = $a; $i = 0; do { $x = $x + 1; } while (++$i < 4); return $x; }
function f2($a) { $x = $a; $i = 0; do { $x = $x + 2; } while (++$i < 4); return $x; }
function f3($a) { $x = $a; $i = 0; do { $x = $x + 3; } while (++$i < 4); return $x; }
function f4($a) { $x = $a; $i = 0; do { $x = $x + 4; } while (++$i < 4); return $x; }
function f5($a) { $x = $a; $i = 0; do { $x = $x + 5; } while (++$i < 4); return $x; }
function f6($a) { $x = $a; $i = 0; do { $x = $x + 6; } while (++$i < 4); return $x; }
function f7($a) { $x = $a; $i = 0; do { $x = $x + 7; } while (++$i < 4); return $x; }
function f8($a) { $x = $a; $i = 0; do { $x = $x + 8; } while (++$i < 4); return $x; }
function f9($a) { $x = $a; $i = 0; do { $x = $x + 9; } while (++$i < 4); return $x; }

$sum = 0;
foreach ([1, '1', 1.5, 2] as $v) {
    for ($j = 0; $j < 300; $j++) {
        $sum += f0($v) + f1($v) + f2($v) + f3($v) + f4($v)
            + f5($v) + f6($v) + f7($v) + f8($v) + f9($v);
    }
}
var_dump($sum);
?>
--EXPECT--
float(232500)
