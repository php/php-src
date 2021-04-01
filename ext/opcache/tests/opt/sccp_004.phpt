--TEST--
SCCP 004: Conditional Constant Propagation of non-escaping array elements
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
    $a = [1,2,3];
    $a[2] = $x;
    $i = 1;
    $c = $i < 2;
    if ($c) {
        $k = 2 * $i;
        $a[$k] = $i;
//		$a[$k]++;
        echo isset($a[$k]);
//		$a[$k] += 5;
    }
    echo $a[2];
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %ssccp_004.php:1-17
0000 RETURN int(1)

foo:
     ; (lines=4, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %ssccp_004.php:2-15
0000 CV0($x) = RECV 1
0001 ECHO string("1")
0002 ECHO string("1")
0003 RETURN null
