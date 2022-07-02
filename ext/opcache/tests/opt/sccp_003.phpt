--TEST--
SCCP 003: Conditional Constant Propagation of non-escaping array elements
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
    $a = [1,2,3];
    $i = 1;
    $c = $i < 2;
    if ($c) {
        $k = 2 * $i;
        $a[$k] = $i;
        echo $a[$k];
    }
    echo $a[2];
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_003.php:1-14
0000 RETURN int(1)

foo:
     ; (lines=3, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_003.php:2-12
0000 ECHO string("1")
0001 ECHO string("1")
0002 RETURN null
