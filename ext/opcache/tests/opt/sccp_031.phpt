--TEST--
SCCP 031: Echo optimizations
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
function foo() {
    $k = 0;
    $a = [null];
    echo isset($a[$k]);
    echo "b";
    echo isset($a[$k+1]);
    echo "c";
    echo $a[$k];
    echo $a; // Should not be optimized
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_031.php:1-13
0000 RETURN int(1)

foo:
     ; (lines=4, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s_031.php:2-11
0000 ECHO string("b")
0001 ECHO string("c")
0002 ECHO array(...)
0003 RETURN null
