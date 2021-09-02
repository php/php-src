--TEST--
Test that internal methods can be optimized based on zend_func_infos.h
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
opcache.preload=
--EXTENSIONS--
opcache
zend_test
--FILE--
<?php

class Test extends _ZendTestTypeInference {
    public function test1(): int {
        return $this->getIntArray()[0] ?? 0;
    }

    public static function test2(): int {
        return $this->createIntArray()[0] ?? 0;
    }
}

?>
--EXPECTF--
$_main:
     ; (lines=2, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 DECLARE_CLASS_DELAYED string("test") string("_zendtesttypeinference")
0001 RETURN int(1)

Test::test1:
     ; (lines=7, args=0, vars=0, tmps=2)
     ; (after optimizer)
     ; %s
0000 INIT_METHOD_CALL 0 THIS string("getIntArray")
0001 V0 = DO_FCALL
0002 T1 = FETCH_DIM_IS V0 int(0)
0003 T0 = COALESCE T1 0005
0004 T0 = QM_ASSIGN int(0)
0005 VERIFY_RETURN_TYPE T0
0006 RETURN T0
LIVE RANGES:
     0: 0005 - 0006 (tmp/var)

Test::test2:
     ; (lines=8, args=0, vars=0, tmps=2)
     ; (after optimizer)
     ; %s
0000 V0 = FETCH_THIS
0001 INIT_METHOD_CALL 0 V0 string("createIntArray")
0002 V0 = DO_FCALL
0003 T1 = FETCH_DIM_IS V0 int(0)
0004 T0 = COALESCE T1 0006
0005 T0 = QM_ASSIGN int(0)
0006 VERIFY_RETURN_TYPE T0
0007 RETURN T0
LIVE RANGES:
     0: 0006 - 0007 (tmp/var)
