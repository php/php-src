--TEST--
Nullsafe e-ssa pi node placement
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x200000
--EXTENSIONS--
opcache
--FILE--
<?php

class Test {
    public int $prop;
}

function test(?Test $test) {
    var_dump($test?->prop);
}

?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0, ssa_vars=0, no_loops)
     ; (before dfa pass)
     ; %s
     ; return  [long] RANGE[1..1]
BB0:
     ; start exit lines=[0-0]
     ; level=0
0000 RETURN int(1)

test:
     ; (lines=7, args=1, vars=1, tmps=2, ssa_vars=6, no_loops)
     ; (before dfa pass)
     ; %s
     ; return  [null] RANGE[0..0]
     ; #0.CV0($test) NOVAL [undef]
BB0:
     ; start lines=[0-2]
     ; to=(BB2, BB1)
     ; level=0
     ; children=(BB1, BB2)
0000 #1.CV0($test) [null, object (instanceof Test)] = RECV 1
0001 INIT_FCALL %d %d %s
0002 #2.T1 [null] = JMP_NULL #1.CV0($test) [null, object (instanceof Test)] BB2

BB1:
     ; follow lines=[3-3]
     ; from=(BB0)
     ; to=(BB2)
     ; idom=BB0
     ; level=1
     #3.CV0($test) [object (instanceof Test)] = Pi<BB0>(#1.CV0($test) [null, object (instanceof Test)] & TYPE [ref, bool, long, double, string, array of [any, ref], object, resource])
0003 #4.T1 [long] = FETCH_OBJ_R #3.CV0($test) [object (instanceof Test)] string("prop")

BB2:
     ; follow target exit lines=[4-6]
     ; from=(BB0, BB1)
     ; idom=BB0
     ; level=1
     #5.X1 [null, long] = Phi(#2.X1 [null], #4.X1 [long])
0004 SEND_VAL #5.T1 [null, long] 1
0005 DO_ICALL
0006 RETURN null
