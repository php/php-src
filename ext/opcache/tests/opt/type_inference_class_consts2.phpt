--TEST--
Test type inference of class consts - reference by parent
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
}

final class Test2 extends Test1 {
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

?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(1)

Test2::getParentFoo:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)

Test2::getParentBar:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)

Test2::getParentBaz:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)
