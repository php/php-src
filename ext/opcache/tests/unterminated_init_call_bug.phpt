--TEST--
Mark unterminated INIT_CALL and SEND_VAL instructions as NOP when removing basic blocks
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

var_dump(
    var_dump('foo'),
    exit()
);

--EXPECTF--
$_main:
     ; (lines=6, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 NOP
0001 INIT_FCALL 1 96 string("var_dump")
0002 SEND_VAL string("foo") 1
0003 V0 = DO_ICALL
0004 NOP
0005 EXIT
string(3) "foo"
