--TEST--
#[\NoDiscard]: Functions with known-used result use DO_[IU]CALL opcodes
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
--EXTENSIONS--
opcache
zend_test
--FILE--
<?php

zend_test_nodiscard();
(void)zend_test_nodiscard();
$success = zend_test_nodiscard();

#[\NoDiscard]
function test() {
	return new stdClass();
}

test();
(void)test();
$obj = test();

?>
--EXPECTF--
$_main:
     ; (lines=17, args=0, vars=2, tmps=%d)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 0 %d string("zend_test_nodiscard")
0001 DO_FCALL_BY_NAME
0002 INIT_FCALL 0 %d string("zend_test_nodiscard")
0003 V2 = DO_ICALL
0004 FREE V2
0005 INIT_FCALL 0 %d string("zend_test_nodiscard")
0006 V2 = DO_ICALL
0007 ASSIGN CV0($success) V2
0008 INIT_FCALL 0 %d string("test")
0009 DO_FCALL_BY_NAME
0010 INIT_FCALL 0 %d string("test")
0011 V2 = DO_UCALL
0012 FREE V2
0013 INIT_FCALL 0 %d string("test")
0014 V2 = DO_UCALL
0015 ASSIGN CV1($obj) V2
0016 RETURN int(1)

test:
     ; (lines=3, args=0, vars=0, tmps=%d)
     ; (after optimizer)
     ; %s
0000 V0 = NEW 0 string("stdClass")
0001 DO_FCALL
0002 RETURN V0
LIVE RANGES:
     0: 0001 - 0002 (new)

Warning: The return value of function zend_test_nodiscard() should either be used or intentionally ignored by casting it as (void), custom message in %s on line %d

Warning: The return value of function test() should either be used or intentionally ignored by casting it as (void) in %s on line %d
