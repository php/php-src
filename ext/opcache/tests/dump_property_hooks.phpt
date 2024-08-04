--TEST--
Optimizer optimizes hooks, OpCache dump emits them
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--EXTENSIONS--
opcache
--FILE--
<?php

class A {
    public int $prop {
        get {
            return 42;
            echo 'Dead code';
        }
        set {
            echo "Setting $value";
            return;
            echo 'Dead code';
        }
    }
}

$a = new A();
var_dump($a->prop);
$a->prop = 41;

?>
--EXPECTF--
$_main:
     ; (lines=10, args=0, vars=1, tmps=%d)
     ; (after optimizer)
     ; %sdump_property_hooks.php:1-22
0000 V1 = NEW 0 string("A")
0001 DO_FCALL
0002 ASSIGN CV0($a) V1
0003 INIT_FCALL 1 %d string("var_dump")
0004 T1 = FETCH_OBJ_R CV0($a) string("prop")
0005 SEND_VAL T1 1
0006 DO_ICALL
0007 ASSIGN_OBJ CV0($a) string("prop")
0008 OP_DATA int(41)
0009 RETURN int(1)
LIVE RANGES:
     1: 0001 - 0002 (new)

A::$prop::get:
     ; (lines=1, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %sdump_property_hooks.php:5-8
0000 RETURN int(42)

A::$prop::set:
     ; (lines=4, args=1, vars=1, tmps=%d)
     ; (after optimizer)
     ; %sdump_property_hooks.php:9-13
0000 CV0($value) = RECV 1
0001 T1 = FAST_CONCAT string("Setting ") CV0($value)
0002 ECHO T1
0003 RETURN null
int(42)
Setting 41
