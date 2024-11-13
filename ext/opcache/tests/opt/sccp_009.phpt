--TEST--
SCCP 009: Conditional Constant Propagation of non-escaping array elements
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
    $a[0] = $x;
    $a[1] = 2;
    echo $a[1];
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_009.php:1-8
0000 RETURN int(1)

foo:
     ; (lines=3, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %ssccp_009.php:2-6
0000 CV0($x) = RECV 1
0001 ECHO string("2")
0002 RETURN null
