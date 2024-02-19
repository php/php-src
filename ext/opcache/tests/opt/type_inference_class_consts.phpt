--TEST--
Test type inference of class consts
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

class Test1 {
    public const FOO = 42;
    public final const BAR = 42;
    public const int BAZ = 42;

    public function getSelfFoo(): int {
        return self::FOO;
    }

    public function getSelfBar(): int {
        return self::BAR;
    }

    public function getSelfBaz(): int {
        return self::BAZ;
    }

    public function getStaticFoo(): int {
        return static::FOO;
    }

    public function getStaticBar(): int {
        return static::BAR;
    }

    public function getStaticBaz(): int {
        return static::BAZ;
    }
}

final class Test2 extends Test1 {
    public function getStaticFoo(): int {
        return static::FOO;
    }

    public function getParentFoo(): int {
        return parent::FOO;
    }

    public function getParentBar(): int {
        return parent::BAR;
    }

    public function getParentBaz(): int {
        return parent::BAZ;
    }
}

class Test3 {
    public function getTestFoo(): int {
        return Test1::FOO;
    }

    public function getTestBar(): int {
        return Test1::BAR;
    }

    public function getTestBaz(): int {
        return Test1::BAZ;
    }
}

?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(1)

Test1::getSelfFoo:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)

Test1::getSelfBar:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)

Test1::getSelfBaz:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)

Test1::getStaticFoo:
     ; (lines=3, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 T0 = FETCH_CLASS_CONSTANT (static) (exception) string("FOO")
0001 VERIFY_RETURN_TYPE T0
0002 RETURN T0
LIVE RANGES:
     0: 0001 - 0002 (tmp/var)

Test1::getStaticBar:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)

Test1::getStaticBaz:
     ; (lines=2, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 T0 = FETCH_CLASS_CONSTANT (static) (exception) string("BAZ")
0001 RETURN T0

Test2::getStaticFoo:
     ; (lines=3, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 T0 = FETCH_CLASS_CONSTANT (static) (exception) string("FOO")
0001 VERIFY_RETURN_TYPE T0
0002 RETURN T0
LIVE RANGES:
     0: 0001 - 0002 (tmp/var)

Test2::getParentFoo:
     ; (lines=3, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 T0 = FETCH_CLASS_CONSTANT (parent) (exception) string("FOO")
0001 VERIFY_RETURN_TYPE T0
0002 RETURN T0
LIVE RANGES:
     0: 0001 - 0002 (tmp/var)

Test2::getParentBar:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)

Test2::getParentBaz:
     ; (lines=2, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 T0 = FETCH_CLASS_CONSTANT (parent) (exception) string("BAZ")
0001 RETURN T0

Test3::getTestFoo:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)

Test3::getTestBar:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)

Test3::getTestBaz:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)
