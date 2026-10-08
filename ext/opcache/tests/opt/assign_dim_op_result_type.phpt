--TEST--
Type inference of ASSIGN_DIM_OP result uses the container and key types
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x400000
opcache.preload=
--EXTENSIONS--
opcache
--FILE--
<?php
function f(int $p, int $q, int $k) {
    $a = [$p, $q];
    $r = ($a[$k] += 1);
    return $r;
}

function g(int $p, int $q, int $k) {
    $a = [[$p], [$q]];
    $r = ($a[$k] += [1]);
    return $r;
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=%d, ssa_vars=0, no_loops)
     ; (after dfa pass)
     ; %s
     ; return  [long] RANGE[1..1]
BB0:
     ; start exit lines=[0-0]
     ; level=0
0000 RETURN int(1)

f:
     ; (lines=9, args=3, vars=5, tmps=%d, ssa_vars=14, no_loops)
     ; (after dfa pass)
     ; %s
     ; return  [long, double]
     ; #0.CV0($p) NOVAL [undef]
     ; #1.CV1($q) NOVAL [undef]
     ; #2.CV2($k) NOVAL [undef]
     ; #3.CV3($a) NOVAL [undef]
     ; #4.CV4($r) NOVAL [undef]
BB0:
     ; start exit lines=[0-8]
     ; level=0
0000 #5.CV0($p) [long] RANGE[MIN..MAX] = RECV 1
0001 #6.CV1($q) [long] RANGE[MIN..MAX] = RECV 2
0002 #7.CV2($k) [long] RANGE[MIN..MAX] = RECV 3
0003 #8.T5 NOESC [[packed, hash] array [long] of [long]] = INIT_ARRAY 2 (packed) #5.CV0($p) [long] RANGE[MIN..MAX] NEXT
0004 ADD_ARRAY_ELEMENT #6.CV1($q) [long] RANGE[MIN..MAX] NEXT #8.T5 NOESC [[packed, hash] array [long] of [long]] -> #9.T5 NOESC [[packed, hash] array [long] of [long]]
0005 #10.CV3($a) NOESC [[packed, hash] array [long] of [long]] = QM_ASSIGN #9.T5 NOESC [[packed, hash] array [long] of [long]]
0006 #13.CV4($r) [long, double] = ASSIGN_DIM_OP (ADD) #10.CV3($a) NOESC [[packed, hash] array [long] of [long]] -> #11.CV3($a) NOVAL NOESC [[packed, hash] array [long] of [long, double]] #7.CV2($k) [long] RANGE[MIN..MAX]
0007 OP_DATA int(1)
0008 RETURN #13.CV4($r) [long, double]

g:
     ; (lines=11, args=3, vars=5, tmps=%d, ssa_vars=16, no_loops)
     ; (after dfa pass)
     ; %s
     ; return  [long, double, array of [any, ref]]
     ; #0.CV0($p) NOVAL [undef]
     ; #1.CV1($q) NOVAL [undef]
     ; #2.CV2($k) NOVAL [undef]
     ; #3.CV3($a) NOVAL [undef]
     ; #4.CV4($r) NOVAL [undef]
BB0:
     ; start exit lines=[0-10]
     ; level=0
0000 #5.CV0($p) [long] RANGE[MIN..MAX] = RECV 1
0001 #6.CV1($q) [long] RANGE[MIN..MAX] = RECV 2
0002 #7.CV2($k) [long] RANGE[MIN..MAX] = RECV 3
0003 #8.T5 NOESC [[packed, hash] array [long] of [long]] = INIT_ARRAY 1 (packed) #5.CV0($p) [long] RANGE[MIN..MAX] NEXT
0004 #9.T6 NOESC [[packed, hash] array [long] of [array]] = INIT_ARRAY 2 (packed) #8.T5 NOESC [[packed, hash] array [long] of [long]] NEXT
0005 #10.T7 NOESC [[packed, hash] array [long] of [long]] = INIT_ARRAY 1 (packed) #6.CV1($q) [long] RANGE[MIN..MAX] NEXT
0006 ADD_ARRAY_ELEMENT #10.T7 NOESC [[packed, hash] array [long] of [long]] NEXT #9.T6 NOESC [[packed, hash] array [long] of [array]] -> #11.T6 NOESC [[packed, hash] array [long] of [array]]
0007 #12.CV3($a) NOESC [[packed, hash] array [long] of [array]] = QM_ASSIGN #11.T6 NOESC [[packed, hash] array [long] of [array]]
0008 #15.CV4($r) [long, double, array of [any, ref]] = ASSIGN_DIM_OP (ADD) #12.CV3($a) NOESC [[packed, hash] array [long] of [array]] -> #13.CV3($a) NOVAL NOESC [[packed, hash] array [long] of [long, double, array]] #7.CV2($k) [long] RANGE[MIN..MAX]
0009 OP_DATA array(...)
0010 RETURN #15.CV4($r) [long, double, array of [any, ref]]
