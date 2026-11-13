--TEST--
GH-23983 (Function JIT loses register values when an interrupt is handled at a loop header)
--EXTENSIONS--
opcache
pcntl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=function
opcache.jit_buffer_size=64M
--CREDITS--
frodeborli
--FILE--
<?php
$n = 0;
pcntl_async_signals(true);
pcntl_signal(SIGALRM, function () use (&$n) { $n++; });
pcntl_alarm(1);
function f(&$n) {
    $x = 7;
    for ($i = 1; $n < 1; $i++) {
        $x = ($x * 31 + $i) & 0xffffff;
    }
    $y = 7;
    for ($j = 1; $j < $i; $j++) {
        $y = ($y * 31 + $j) & 0xffffff;
    }
    return $x === $y;
}
var_dump(f($n));
?>
--EXPECT--
bool(true)
