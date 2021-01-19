--TEST--
infer 001: '==='/'!==' optimizations
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
opcache.preload=
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo(int $a, ?int $nullable = null) {
    if ($a === $undef) {
        echo "Impossible\n";
    } else {
        echo "Always reached\n";
    }
    if ($nullable === $undef2) {
        echo "Possible\n";
    } else {
        echo "Null\n";
    }

}
foo(1);
foo(1, 1);
?>
--EXPECTF--
$_main:
     ; (lines=8, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %sinfer_001.php:1-18
0000 INIT_FCALL 1 160 string("foo")
0001 SEND_VAL int(1) 1
0002 DO_UCALL
0003 INIT_FCALL 2 160 string("foo")
0004 SEND_VAL int(1) 1
0005 SEND_VAL int(1) 2
0006 DO_UCALL
0007 RETURN int(1)

foo:
     ; (lines=13, args=2, vars=4, tmps=1)
     ; (after optimizer)
     ; %sinfer_001.php:2-14
0000 CV0($a) = RECV 1
0001 CV1($nullable) = RECV_INIT 2 null
0002 T4 = IS_IDENTICAL CV0($a) CV2($undef)
0003 JMP 0006
0004 ECHO string("Impossible
")
0005 JMP 0007
0006 ECHO string("Always reached
")
0007 T4 = IS_IDENTICAL CV1($nullable) CV3($undef2)
0008 JMPZ T4 0011
0009 ECHO string("Possible
")
0010 RETURN null
0011 ECHO string("Null
")
0012 RETURN null

Warning: Undefined variable $undef in %sinfer_001.php on line 3
Always reached

Warning: Undefined variable $undef2 in %sinfer_001.php on line 8
Possible

Warning: Undefined variable $undef in %sinfer_001.php on line 3
Always reached

Warning: Undefined variable $undef2 in %sinfer_001.php on line 8
Null
