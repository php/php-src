--TEST--
SCCP 010: Conditional Constant Propagation of non-escaping object properties
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
function foo() {
    $o = new stdClass();
    $o->foo = 0;
    $i = 1;
    $c = $i < 2;
    if ($c) {
        $k = 2 * $i;
        $o->foo = $i;
        echo $o->foo;
    }
    echo $o->foo;
}
?>
--EXPECTF--
$_main:
     ; (lines=2, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_010.php:1-15
0000 DEFER_RUN
0001 RETURN int(1)

foo:
     ; (lines=4, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_010.php:2-13
0000 ECHO string("1")
0001 ECHO string("1")
0002 DEFER_RUN
0003 RETURN null
