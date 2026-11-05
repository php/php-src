--TEST--
JIT: trace buffer overflow while recording fake init calls on recursive return
--INI--
opcache.jit_max_trace_length=19
--FILE--
<?php

function id($x) { return $x; }

/* The return trace started after the recursive call of rec() unrolls one
 * return. At that point the parent rec() frame still has the two id() calls
 * under construction, and recording a fake init call for them overflows the
 * trace buffer. */
function rec($n) {
    if ($n <= 0) {
        return 0;
    }
    return id(id(rec($n - 1)));
}

$s = 0;
for ($i = 0; $i < 50; $i++) {
    $s += rec(10);
}

var_dump($s);

?>
--EXPECT--
int(0)
