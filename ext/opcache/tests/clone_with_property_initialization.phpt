--TEST--
Clone with property initialization
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

function test() {
    $class = new stdClass();
    $class->a = 1;
    $class->b = "";

    return clone $class with {a: 2, b: "abc"};
}

?>
--EXPECTF--
$_main:
     ; %s
     ; (after optimizer)
     ; %s
0000 RETURN int(1)

test:
     ; %s
     ; (after optimizer)
     ; %s
0000 V1 = NEW 0 string("stdClass")
0001 DO_FCALL
0002 CV0($class) = QM_ASSIGN V1
0003 ASSIGN_OBJ CV0($class) string("a")
0004 OP_DATA int(1)
0005 ASSIGN_OBJ CV0($class) string("b")
0006 OP_DATA string("")
0007 V1 = CLONE CV0($class)
0008 V2 = INIT_OBJ V1 string("a")
0009 OP_DATA int(2)
0010 V1 = INIT_OBJ V2 string("b")
0011 OP_DATA string("abc")
0012 RETURN V1
LIVE RANGES:
%s
%s
%s
