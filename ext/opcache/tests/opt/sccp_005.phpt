--TEST--
SCCP 005: Conditional Constant Propagation of non-escaping array elements
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
    $a = [1,2,$x];
    echo $a[1];
}
?>
--EXPECTF--
$_main:
     ; (lines=2, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_005.php:1-7
0000 DEFER_RUN
0001 RETURN int(1)

foo:
     ; (lines=4, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %ssccp_005.php:2-5
0000 CV0($x) = RECV 1
0001 ECHO string("2")
0002 DEFER_RUN
0003 RETURN null
