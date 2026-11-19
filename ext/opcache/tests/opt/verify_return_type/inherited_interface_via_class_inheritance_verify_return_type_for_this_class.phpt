--TEST--
Return type check elision for inherited interface via class extension return type and $this in class method
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x30000
--EXTENSIONS--
opcache
--FILE--
<?php
interface I1 {}
interface I2 extends I1 {}

class C1 implements I2 {}

class C2 extends C1 {
    public function foo(): I2 {
        return $this;
    }
}

?>
--EXPECTF--
$_main:
     ; (lines=4, args=0, vars=0, tmps=0)
     ; (before optimizer)
     ; %s:1-14
     ; return  [] RANGE[0..0]
0000 DECLARE_CLASS string("i2")
0001 DECLARE_CLASS string("c1")
0002 DECLARE_CLASS_DELAYED string("c2") string("c1")
0003 RETURN int(1)

C2::foo:
     ; (lines=5, args=0, vars=0, tmps=1)
     ; (before optimizer)
     ; %s:8-10
     ; return  [] RANGE[0..0]
0000 T0 = FETCH_THIS
0001 VERIFY_RETURN_TYPE T0
0002 RETURN T0
0003 VERIFY_RETURN_TYPE
0004 RETURN null
LIVE RANGES:
     0: 0001 - 0002 (tmp/var)

$_main:
     ; (lines=4, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s:1-14
0000 DECLARE_CLASS string("i2")
0001 DECLARE_CLASS string("c1")
0002 DECLARE_CLASS_DELAYED string("c2") string("c1")
0003 RETURN int(1)

C2::foo:
     ; (lines=3, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s:8-10
0000 T0 = FETCH_THIS
0001 VERIFY_RETURN_TYPE T0
0002 RETURN T0
LIVE RANGES:
     0: 0001 - 0002 (tmp/var)
