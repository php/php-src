--TEST--
JIT: trace buffer overflow while recording fake init calls at trace start
--INI--
opcache.jit_max_trace_length=8
--FILE--
<?php

class C { static function f($x) { return $x + 1; } }

function sink(array $a) { return array_sum($a); }
function id($x) { return $x; }

/* array_map() is compiled to a foreach loop, so the loop header sits between
 * the INIT_FCALL of sink()/id() and their DO_UCALL. A root loop trace started
 * there has to record a fake init call for each of the 6 pending calls, which
 * does not fit into a trace buffer limited to 8 records. The recording must be
 * aborted; the partially recorded trace must not be compiled. */
function test(array $a, $o) {
    return sink(id(id(id(id(id(array_map($o::f(...), $a)))))));
}

$a = [1, 2, 3];
$o = new C();

for ($i = 0; $i < 5; $i++) {
    $r = test($a, $o);
}

var_dump($r);

?>
--EXPECT--
int(9)
