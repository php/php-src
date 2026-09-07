--TEST--
JIT: array_map() foreach optimization: loop trace inside a call region must not clobber EX(call)
--EXTENSIONS--
opcache
--FILE--
<?php

class C {
    static function f($x) { return $x + 1; }
}

function sink(array $a) { return array_sum($a); }

/* array_map() is compiled to a foreach loop, so both loop headers sit between
 * the INIT_FCALL of sink() and its DO_UCALL. The side trace recorded at the
 * exit of the first loop runs into the header of the second one and is turned
 * into a loop trace rooted there; that conversion must not lose track of the
 * sink() frame that is still under construction, otherwise DO_FCALL of the
 * callback stores NULL into EX(call) and the following SEND_VAL crashes.
 *
 * $o::f(...) is used so that INIT_STATIC_METHOD_CALL falls back to the VM
 * handler, which is what makes the JIT emit the EX(call) store at DO_FCALL. */
function test(array $a, $o) {
    $n  = sink(array_map($o::f(...), $a));
    $n += sink(array_map($o::f(...), $a));
    return $n;
}

$a = [1, 2, 3];
$o = new C();

for ($i = 0; $i < 2; $i++) {
    $r = test($a, $o);
}

var_dump($r);

?>
--EXPECT--
int(18)
