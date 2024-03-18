--TEST--
Test type inference of class consts - interface consts
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
opcache.preload=
--EXTENSIONS--
opcache
--FILE--
<?php

interface TestI {
    public const int A = 42;
    public final const B = 42;
}

class Test4 implements TestI {
    public function getSelfA(): int {
        return self::A;
    }

    public function getSelfB(): int {
        return self::B;
    }

    public function getStaticA(): int {
        return static::A;
    }

    public function getStaticB(): int {
        return static::B;
    }

    public function getTestIA(): int {
        return TestI::A;
    }

    public function getTestIB(): int {
        return TestI::B;
    }
}

?>
--EXPECTF--
$_main:
     ; (lines=2, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 DECLARE_CLASS string("test4")
0001 RETURN int(1)

Test4::getSelfA:
     ; (lines=3, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 T0 = FETCH_CLASS_CONSTANT (self) (exception) string("A")
0001 VERIFY_RETURN_TYPE T0
0002 RETURN T0
LIVE RANGES:
     0: 0001 - 0002 (tmp/var)

Test4::getSelfB:
     ; (lines=3, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 T0 = FETCH_CLASS_CONSTANT (self) (exception) string("B")
0001 VERIFY_RETURN_TYPE T0
0002 RETURN T0
LIVE RANGES:
     0: 0001 - 0002 (tmp/var)

Test4::getStaticA:
     ; (lines=3, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 T0 = FETCH_CLASS_CONSTANT (static) (exception) string("A")
0001 VERIFY_RETURN_TYPE T0
0002 RETURN T0
LIVE RANGES:
     0: 0001 - 0002 (tmp/var)

Test4::getStaticB:
     ; (lines=3, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 T0 = FETCH_CLASS_CONSTANT (static) (exception) string("B")
0001 VERIFY_RETURN_TYPE T0
0002 RETURN T0
LIVE RANGES:
     0: 0001 - 0002 (tmp/var)

Test4::getTestIA:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)

Test4::getTestIB:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)
