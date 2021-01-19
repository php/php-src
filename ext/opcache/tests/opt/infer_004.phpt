--TEST--
Infer 004: '==='/'!==' optimizations
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
function foo(int $a, stdClass $b) {
    return [$a === $b, $a !== $b, $b === 2];
}
var_export(foo(1, new stdClass()));
?>
--EXPECTF--
$_main:
     ; (lines=10, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s:1-7
0000 INIT_FCALL 1 %d string("var_export")
0001 INIT_FCALL 2 %d string("foo")
0002 SEND_VAL int(1) 1
0003 V0 = NEW 0 string("stdClass")
0004 DO_FCALL
0005 SEND_VAR V0 2
0006 V0 = DO_UCALL
0007 SEND_VAR V0 1
0008 DO_ICALL
0009 RETURN int(1)
LIVE RANGES:
     0: 0004 - 0005 (new)

foo:
     ; (lines=6, args=2, vars=2, tmps=1)
     ; (after optimizer)
     ; %s:2-4
0000 CV0($a) = RECV 1
0001 CV1($b) = RECV 2
0002 T2 = INIT_ARRAY 3 (packed) bool(false) NEXT
0003 T2 = ADD_ARRAY_ELEMENT bool(true) NEXT
0004 T2 = ADD_ARRAY_ELEMENT bool(false) NEXT
0005 RETURN T2
LIVE RANGES:
     2: 0003 - 0005 (tmp/var)
array (
  0 => false,
  1 => true,
  2 => false,
)
