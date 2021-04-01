--TEST--
SCCP 007: Conditional Constant Propagation of non-escaping array elements
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
    echo $a[0];
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_007.php:1-11
0000 RETURN int(1)

foo:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %ssccp_007.php:2-9
0000 CV0($x) = RECV 1
0001 ECHO string("0")
0002 RETURN null
