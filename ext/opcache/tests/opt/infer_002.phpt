--TEST--
Infer 002: '==='/'!==' optimizations
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
function foo(int $a, object $b) {
    for ($i = 0; $i < 2; $i++) {
        if ($a !== $b) {
            $a = $b;
        }
    }
    return [$a === '2', $b !== 2];  // TODO: Merge the INIT_ARRAY and ADD_ARRAY_ELEMENT?
}
var_export(foo(1, new stdClass()));
?>
--EXPECTF--
$_main:
     ; (lines=10, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s:1-12
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
     ; (lines=13, args=2, vars=3, tmps=1)
     ; (after optimizer)
     ; %s:2-9
0000 CV0($a) = RECV 1
0001 CV1($b) = RECV 2
0002 CV2($i) = QM_ASSIGN int(0)
0003 JMP 0008
0004 T3 = IS_NOT_IDENTICAL CV0($a) CV1($b)
0005 JMPZ T3 0007
0006 ASSIGN CV0($a) CV1($b)
0007 PRE_INC CV2($i)
0008 T3 = IS_SMALLER CV2($i) int(2)
0009 JMPNZ T3 0004
0010 T3 = INIT_ARRAY 2 (packed) bool(false) NEXT
0011 T3 = ADD_ARRAY_ELEMENT bool(true) NEXT
0012 RETURN T3
LIVE RANGES:
     3: 0011 - 0012 (tmp/var)
array (
  0 => false,
  1 => true,
)