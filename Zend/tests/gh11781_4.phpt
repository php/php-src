--TEST--
GH-11781: Constant property assignment
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php
class Foo {
    public $array = [];
}
const FOO = new Foo();
var_dump(FOO->array[] = 42);
?>
--EXPECTF--
$_main:
     ; (lines=9, args=0, vars=0, tmps=2)
     ; (after optimizer)
     ; %sgh11781_4.php:1-8
0000 DECLARE_CONST string("FOO") zval(type=11)
0001 INIT_FCALL 1 96 string("var_dump")
0002 V0 = FETCH_CONSTANT string("FOO")
0003 V1 = FETCH_OBJ_W (dim write) V0 string("array")
0004 T0 = ASSIGN_DIM V1 NEXT
0005 OP_DATA int(42)
0006 SEND_VAL T0 1
0007 DO_ICALL
0008 RETURN int(1)
LIVE RANGES:
     0: 0005 - 0006 (tmp/var)
int(42)
