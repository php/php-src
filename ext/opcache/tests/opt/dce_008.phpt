--TEST--
DCE 008: Escaping of enclosed arrays doesn't prevent removal of enclosing array
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
function esc(int $x) {
        $a[0] = $x;
        $b[0] = $a;
        return $a;
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %sdce_008.php:1-8
0000 RETURN int(1)

esc:
     ; (lines=4, args=1, vars=2, tmps=0)
     ; (after optimizer)
     ; %sdce_008.php:2-6
0000 CV0($x) = RECV 1
0001 ASSIGN_DIM CV1($a) int(0)
0002 OP_DATA CV0($x)
0003 RETURN CV1($a)
