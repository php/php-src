--TEST--
GH-11170 (Too wide OR and AND range inferred)
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
function test_or() {
    if (rand() % 10) {
        $a = -27;
        $b = -20;
    } else {
        $a = -7;
        $b = -10;
    }

    return $a | $b;
}

function test_and() {
    if (rand() % 10) {
        $a = -12;
        $b = -27;
    } else {
        $a = -9;
        $b = -25;
    }

    return $a & $b;
}

test_or();
test_and();
?>
--EXPECTF--
$_main:
     ; (lines=5, args=0, vars=0, tmps=2, ssa_vars=0, no_loops)
     ; (after dfa pass)
     ; %s
     ; return  [long] RANGE[1..1]
BB0:
     ; start exit lines=[0-4]
     ; level=0
0000 INIT_FCALL 0 %d string("test_or")
0001 DO_UCALL
0002 INIT_FCALL 0 %d string("test_and")
0003 DO_UCALL
0004 RETURN int(1)

test_or:
     ; (lines=11, args=0, vars=2, tmps=7, ssa_vars=11, no_loops)
     ; (after dfa pass)
     ; %s
     ; return  [long] RANGE[-20..-1]
     ; #0.CV0($a) NOVAL [undef]
     ; #1.CV1($b) NOVAL [undef]
BB0:
     ; start lines=[0-3]
     ; to=(BB2, BB1)
     ; level=0
     ; children=(BB1, BB2, BB3)
0000 INIT_FCALL %d %d %s
0001 #2.V2 [long] = DO_ICALL
0002 #3.T3 [long] RANGE[MIN..MAX] = MOD #2.V2 [long] int(10)
0003 JMPZ #3.T3 [long] RANGE[MIN..MAX] BB2

BB1:
     ; follow lines=[4-6]
     ; from=(BB0)
     ; to=(BB3)
     ; idom=BB0
     ; level=1
0004 #4.CV0($a) [long] RANGE[-27..-27] = QM_ASSIGN int(-27)
0005 #5.CV1($b) [long] RANGE[-20..-20] = QM_ASSIGN int(-20)
0006 JMP BB3

BB2:
     ; target lines=[7-8]
     ; from=(BB0)
     ; to=(BB3)
     ; idom=BB0
     ; level=1
0007 #6.CV0($a) [long] RANGE[-7..-7] = QM_ASSIGN int(-7)
0008 #7.CV1($b) [long] RANGE[-10..-10] = QM_ASSIGN int(-10)

BB3:
     ; follow target exit lines=[9-10]
     ; from=(BB1, BB2)
     ; idom=BB0
     ; level=1
     #8.CV0($a) [long] RANGE[-27..-7] = Phi(#4.CV0($a) [long] RANGE[-27..-27], #6.CV0($a) [long] RANGE[-7..-7])
     #9.CV1($b) [long] RANGE[-20..-10] = Phi(#5.CV1($b) [long] RANGE[-20..-20], #7.CV1($b) [long] RANGE[-10..-10])
0009 #10.T8 [long] RANGE[-20..-1] = BW_OR #8.CV0($a) [long] RANGE[-27..-7] #9.CV1($b) [long] RANGE[-20..-10]
0010 RETURN #10.T8 [long] RANGE[-20..-1]

test_and:
     ; (lines=11, args=0, vars=2, tmps=7, ssa_vars=11, no_loops)
     ; (after dfa pass)
     ; %s
     ; return  [long] RANGE[-28..-25]
     ; #0.CV0($a) NOVAL [undef]
     ; #1.CV1($b) NOVAL [undef]
BB0:
     ; start lines=[0-3]
     ; to=(BB2, BB1)
     ; level=0
     ; children=(BB1, BB2, BB3)
0000 INIT_FCALL %d %d %s
0001 #2.V2 [long] = DO_ICALL
0002 #3.T3 [long] RANGE[MIN..MAX] = MOD #2.V2 [long] int(10)
0003 JMPZ #3.T3 [long] RANGE[MIN..MAX] BB2

BB1:
     ; follow lines=[4-6]
     ; from=(BB0)
     ; to=(BB3)
     ; idom=BB0
     ; level=1
0004 #4.CV0($a) [long] RANGE[-12..-12] = QM_ASSIGN int(-12)
0005 #5.CV1($b) [long] RANGE[-27..-27] = QM_ASSIGN int(-27)
0006 JMP BB3

BB2:
     ; target lines=[7-8]
     ; from=(BB0)
     ; to=(BB3)
     ; idom=BB0
     ; level=1
0007 #6.CV0($a) [long] RANGE[-9..-9] = QM_ASSIGN int(-9)
0008 #7.CV1($b) [long] RANGE[-25..-25] = QM_ASSIGN int(-25)

BB3:
     ; follow target exit lines=[9-10]
     ; from=(BB1, BB2)
     ; idom=BB0
     ; level=1
     #8.CV0($a) [long] RANGE[-12..-9] = Phi(#4.CV0($a) [long] RANGE[-12..-12], #6.CV0($a) [long] RANGE[-9..-9])
     #9.CV1($b) [long] RANGE[-27..-25] = Phi(#5.CV1($b) [long] RANGE[-27..-27], #7.CV1($b) [long] RANGE[-25..-25])
0009 #10.T8 [long] RANGE[-28..-25] = BW_AND #8.CV0($a) [long] RANGE[-12..-9] #9.CV1($b) [long] RANGE[-27..-25]
0010 RETURN #10.T8 [long] RANGE[-28..-25]
