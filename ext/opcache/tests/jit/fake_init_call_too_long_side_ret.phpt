--TEST--
JIT: trace buffer overflow while recording fake init calls on side trace return
--INI--
opcache.jit_max_trace_length=55
--FILE--
<?php

function id($x) { return $x; }

/* The side trace started at the guard of $a[$k] records the (rarely taken)
 * long branch and returns from g() into outer(), where the two id() calls are
 * still under construction. Recording a fake init call for them overflows the
 * trace buffer at that point. */
function g(array $a, int $k) {
    $x = $a[$k];
    if ($x < 0) {
            $x = $x + 1;
            $x = $x + 2;
            $x = $x + 3;
            $x = $x + 4;
            $x = $x + 5;
            $x = $x + 6;
            $x = $x + 7;
            $x = $x + 8;
            $x = $x + 9;
            $x = $x + 10;
            $x = $x + 11;
            $x = $x + 12;
            $x = $x + 13;
            $x = $x + 14;
            $x = $x + 15;
            $x = $x + 16;
            $x = $x + 17;
            $x = $x + 18;
            $x = $x + 19;
            $x = $x + 20;
            $x = $x + 21;
            $x = $x + 22;
            $x = $x + 23;
            $x = $x + 24;
    }
    return 1;
}

function outer(array $a, int $k) { return id(id(g($a, $k))); }

function driver(array $a) {
    $s = 0;
    for ($i = 0; $i < 300; $i++) {
        $s += outer($a, $i % 11);
    }
    return $s;
}

var_dump(driver([1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1]));

?>
--EXPECT--
int(300)
