--TEST--
DCE 001: Remove dead computation after constants propagation
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
function foo(string $s1, string $s2, string $s3, string $s4) {
    $x = ($s1 . $s2) . ($s3 . $s4);
    $x = 0;
    return $x;
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %sdce_001.php:1-8
0000 RETURN int(1)

foo:
     ; (lines=5, args=4, vars=4, tmps=0)
     ; (after optimizer)
     ; %sdce_001.php:2-6
0000 CV0($s1) = RECV 1
0001 CV1($s2) = RECV 2
0002 CV2($s3) = RECV 3
0003 CV3($s4) = RECV 4
0004 RETURN int(0)
