--TEST--
Type inference test with final class
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
opcache.preload=
zend_test.observer.enabled=0
--EXTENSIONS--
opcache
--FILE--
<?php

final class Test {
    public function getInt(): int {
        return 42;
    }
    public function getInt2(): int {
        return $this->getInt();
    }
}

?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(1)

Test::getInt:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)

Test::getInt2:
     ; (lines=2, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 V0 = QM_ASSIGN int(42)
0001 RETURN V0
