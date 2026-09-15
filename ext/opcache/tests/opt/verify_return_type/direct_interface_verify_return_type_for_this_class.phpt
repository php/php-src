--TEST--
Return type check elision for direct interface return type and $this in class method
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

class C implements I1 {
    public function foo(): I1 {
        return $this;
    }
}

?>
--EXPECTF--
$_main:
     ; (lines=2, args=0, vars=0, tmps=0)
     ; (before optimizer)
     ; %s:1-12
     ; return  [] RANGE[0..0]
0000 DECLARE_CLASS string("c")
0001 RETURN int(1)

C::foo:
     ; (lines=5, args=0, vars=0, tmps=1)
     ; (before optimizer)
     ; %s:6-8
     ; return  [] RANGE[0..0]
0000 T0 = FETCH_THIS
0001 VERIFY_RETURN_TYPE T0
0002 RETURN T0
0003 VERIFY_RETURN_TYPE
0004 RETURN null
LIVE RANGES:
     0: 0001 - 0002 (tmp/var)

$_main:
     ; (lines=2, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s:1-12
0000 DECLARE_CLASS string("c")
0001 RETURN int(1)

C::foo:
     ; (lines=2, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s:6-8
0000 T0 = FETCH_THIS
0001 RETURN T0
