--TEST--
SCCP 006: Conditional Constant Propagation of non-escaping array elements
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
    $a = ["a"=>1,"a"=>2,"a"=>$x];
    echo $a["a"];
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_006.php:1-7
0000 RETURN int(1)

foo:
     ; (lines=8, args=1, vars=2, tmps=1)
     ; (after optimizer)
     ; %ssccp_006.php:2-5
0000 CV0($x) = RECV 1
0001 T2 = INIT_ARRAY 3 int(1) string("a")
0002 T2 = ADD_ARRAY_ELEMENT int(2) string("a")
0003 T2 = ADD_ARRAY_ELEMENT CV0($x) string("a")
0004 CV1($a) = QM_ASSIGN T2
0005 T2 = FETCH_DIM_R CV1($a) string("a")
0006 ECHO T2
0007 RETURN null
LIVE RANGES:
     2: 0002 - 0004 (tmp/var)
