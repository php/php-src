--TEST--
GH-24445 (Tracing JIT: a catch reads an int local as undefined after an internal call throws)
--CREDITS--
EJ-Research
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
--FILE--
<?php
function loop(array $p): void { foreach ($p as $v) {} }
function id(int $n): int { return $n; }
function run(array $p, int $d): int {
    loop($p);
    $n = count($p);
    try {
        intdiv(1, $d);
    } catch (DivisionByZeroError $e) {
        var_dump($n);
        return -1;
    }
    return id($n);
}
for ($i = 0; $i < 10; $i++) {
    run([1], 1);
}
run([1], 0);
?>
--EXPECT--
int(1)
