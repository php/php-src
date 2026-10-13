--TEST--
Return type check elision in the optimizer for an interface reached through a (resolvable) parent class and $this
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x30000
--EXTENSIONS--
opcache
--FILE--
<?php
if (true) {
    class C extends ArrayObject {
        public function foo(): Traversable {
            return $this;
        }
    }
}
?>
--EXPECTF--
$_main:
     ; (lines=3, args=0, vars=0, tmps=0)
     ; (before optimizer)
     ; %s:1-10
     ; return  [] RANGE[0..0]
0000 JMPZ bool(true) 0002
0001 DECLARE_CLASS string("c") string("arrayobject")
0002 RETURN int(1)

C::foo:
     ; (lines=5, args=0, vars=0, tmps=1)
     ; (before optimizer)
     ; %s:4-6
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
     ; %s:1-10
0000 DECLARE_CLASS string("c") string("arrayobject")
0001 RETURN int(1)

C::foo:
     ; (lines=2, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s:4-6
0000 T0 = FETCH_THIS
0001 RETURN T0
