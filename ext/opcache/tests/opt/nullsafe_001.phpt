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
     ; (lines=10, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 1 96 string("var_dump")
0001 SEND_VAL null 1
0002 DO_ICALL
0003 INIT_FCALL 1 96 string("var_dump")
0004 SEND_VAL bool(false) 1
0005 DO_ICALL
0006 INIT_FCALL 1 96 string("var_dump")
0007 SEND_VAL bool(true) 1
0008 DO_ICALL
0009 RETURN null
