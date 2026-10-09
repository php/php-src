--TEST--
Range inference for SPACESHIP
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
function test($a, $b) {
    return ($a <=> $b) + 1;
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

test:
     ; (lines=5, args=2, vars=2, tmps=%d, ssa_vars=6, no_loops)
     ; (after dfa pass)
     ; %s
     ; return  [long] RANGE[0..2]
     ; #0.CV0($a) NOVAL [undef]
     ; #1.CV1($b) NOVAL [undef]
BB0:
     ; start exit lines=[0-4]
     ; level=0
0000 #2.CV0($a) [any] = RECV 1
0001 #3.CV1($b) [any] = RECV 2
0002 #4.T2 [long] RANGE[-1..1] = SPACESHIP #2.CV0($a) [any] #3.CV1($b) [any]
0003 #5.T3 [long] RANGE[0..2] = ADD #4.T2 [long] RANGE[-1..1] int(1)
0004 RETURN #5.T3 [long] RANGE[0..2]
