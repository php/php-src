--TEST--
SCCP 006: Conditional Constant Propagation of non-escaping array elements
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
opcache.preload=
zend_test.observer.enabled=0
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
     ; (lines=5, args=1, vars=2, tmps=1)
     ; (after optimizer)
     ; %ssccp_006.php:2-5
0000 CV0($x) = RECV 1
0001 CV1($a) = ARRAY_DUP 0 array(...) CV0($x)
0002 T2 = FETCH_DIM_R CV1($a) string("a")
0003 ECHO T2
0004 RETURN null
