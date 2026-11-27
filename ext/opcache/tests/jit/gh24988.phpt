--TEST--
GH-24988 (JIT: crash in cleanup_live_vars when % by zero throws mid-expression)
--CREDITS--
waroad
jorgsowa
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

class D {
    function self() { return $this; }
    function __destruct() { echo "dtor\n"; }
}

// Leaves a stale object in the frame slot deep() uses for its first temporary
function pollute($a, $b, $c) {
    return $a->self()->self();
}

function deep(int $a, int $b, int $c) {
    $a &= 255;
    return ($a + 1) - ($b + $b + (($a + 1) % ($b % $c)));
}

$o = new D;
var_dump(deep(15, 3, 7));
pollute($o, 1, 2);
try {
    deep(15, 3, 0);
} catch (DivisionByZeroError $e) {
    echo $e->getMessage(), "\n";
}
echo "end\n";

?>
--EXPECT--
int(9)
Modulo by zero
end
dtor
