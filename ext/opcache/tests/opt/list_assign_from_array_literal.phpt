--TEST--
List assignment from an array literal compiles without the array
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0
opcache.opt_debug_level=0x20000
opcache.preload=
zend_test.observer.enabled=0
--EXTENSIONS--
opcache
--FILE--
<?php
function swap($a, $b) {
    [$a, $b] = [$b, $a];
    return $a;
}
function fibonacci($n) {
    for ($a = 0, $b = 1; $n > 0; $n--, [$a, $b] = [$b, $a + $b]);
    return $a;
}
function elements($p) {
    [$p[0], $p[1]] = [$p[1], $p[0]];
    return $p;
}
function twice($x) {
    [$x, $x] = [$x, 2];
    return $x;
}
?>
--EXPECTF--

$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %slist_assign_from_array_literal.php:1-19
0000 RETURN int(1)

swap:
     ; (lines=8, args=2, vars=2, tmps=4)
     ; (after optimizer)
     ; %slist_assign_from_array_literal.php:2-5
0000 CV0($a) = RECV 1
0001 CV1($b) = RECV 2
0002 T2 = QM_ASSIGN CV1($b)
0003 T3 = QM_ASSIGN CV0($a)
0004 ASSIGN CV0($a) T2
0005 ASSIGN CV1($b) T3
0006 RETURN CV0($a)
0007 RETURN null

fibonacci:
     ; (lines=13, args=1, vars=3, tmps=8)
     ; (after optimizer)
     ; %slist_assign_from_array_literal.php:6-9
0000 CV0($n) = RECV 1
0001 ASSIGN CV1($a) int(0)
0002 ASSIGN CV2($b) int(1)
0003 JMP 0009
0004 PRE_DEC CV0($n)
0005 T6 = QM_ASSIGN CV2($b)
0006 T7 = ADD CV1($a) CV2($b)
0007 ASSIGN CV1($a) T6
0008 ASSIGN CV2($b) T7
0009 T10 = IS_SMALLER int(0) CV0($n)
0010 JMPNZ T10 0004
0011 RETURN CV1($a)
0012 RETURN null

elements:
     ; (lines=9, args=1, vars=1, tmps=4)
     ; (after optimizer)
     ; %slist_assign_from_array_literal.php:10-13
0000 CV0($p) = RECV 1
0001 T1 = FETCH_DIM_R CV0($p) int(1)
0002 T2 = FETCH_DIM_R CV0($p) int(0)
0003 ASSIGN_DIM CV0($p) int(0)
0004 OP_DATA T1
0005 ASSIGN_DIM CV0($p) int(1)
0006 OP_DATA T2
0007 RETURN CV0($p)
0008 RETURN null

twice:
     ; (lines=6, args=1, vars=1, tmps=3)
     ; (after optimizer)
     ; %slist_assign_from_array_literal.php:14-17
0000 CV0($x) = RECV 1
0001 T1 = QM_ASSIGN CV0($x)
0002 ASSIGN CV0($x) T1
0003 ASSIGN CV0($x) int(2)
0004 RETURN CV0($x)
0005 RETURN null