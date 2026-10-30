--TEST--
GH-24988 (JIT: crash in cleanup_live_vars when % by zero throws mid-expression)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=function
opcache.optimization_level=0
--EXTENSIONS--
opcache
--FILE--
<?php

function deep(int $a, int $b, int $c) {
    $a &= 255;
    return ($a + 1) - ($b + $b + (($a + 1) % ($b % $c)));
}

$out = [deep(15, 3, 7)];
try {
    deep(15, 3, 0);
} catch (DivisionByZeroError $e) {
    $out[] = $e->getMessage();
}
var_dump($out);

?>
--EXPECT--
array(2) {
  [0]=>
  int(9)
  [1]=>
  string(14) "Modulo by zero"
}
