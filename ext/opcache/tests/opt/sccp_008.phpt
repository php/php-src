--TEST--
SCCP 008: Conditional Constant Propagation of non-escaping array elements
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
opcache.preload=
--EXTENSIONS--
opcache
--FILE--
<?php
function foo(int $x) {
    if ($x) {
        $a = [0,1];
    } else {
        $a = [0,2];
    }
    echo $a[1];
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_008.php:1-11
0000 RETURN int(1)

foo:
     ; (lines=8, args=1, vars=2, tmps=1)
     ; (after optimizer)
     ; %ssccp_008.php:2-9
0000 CV0($x) = RECV 1
0001 JMPZ CV0($x) 0004
0002 CV1($a) = QM_ASSIGN array(...)
0003 JMP 0005
0004 CV1($a) = QM_ASSIGN array(...)
0005 T2 = FETCH_DIM_R CV1($a) int(1)
0006 ECHO T2
0007 RETURN null
