--TEST--
Nullsafe
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--SKIPIF--
<?php require_once(__DIR__ . '/../skipif.inc'); ?>
--FILE--
<?php

function test() {
    $null = null;
    var_dump($null?->foo);
    var_dump(isset($null?->foo));
    var_dump(empty($null?->foo));
}

?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(1)

test:
     ; (lines=13, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 1 96 string("var_dump")
0001 T0 = JMP_NULL null 0002
0002 SEND_VAL null 1
0003 DO_ICALL
0004 INIT_FCALL 1 96 string("var_dump")
0005 T0 = JMP_NULL null 0006
0006 SEND_VAL bool(false) 1
0007 DO_ICALL
0008 INIT_FCALL 1 96 string("var_dump")
0009 T0 = JMP_NULL null 0010
0010 SEND_VAL bool(true) 1
0011 DO_ICALL
0012 RETURN null
