--TEST--
GH-18107 (Opcache CFG jmp optimization with try-finally breaks the exception table)
--CREDITS--
SpencerMalone
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0x10
opcache.opt_debug_level=0x20000
--FILE--
<?php

if (!isset($badvar)) {
    throw new Exception("Should happen");
}
try {
    goto foo;
} catch (Throwable $e) {
    echo "foo";
    foo:;
} finally {
    throw new Exception("Should not happen");
}

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=0, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 T2 = ISSET_ISEMPTY_CV (isset) CV0($badvar)
0001 JMPNZ T2 0008
0002 V4 = NEW 1 string("Exception")
0003 SEND_VAL_EX string("Should happen") 1
0004 DO_FCALL
0005 THROW V4
0006 CV1($e) = CATCH string("Throwable")
0007 ECHO string("foo")
0008 T6 = FAST_CALL 0010
0009 JMP 0015
0010 V7 = NEW 1 string("Exception")
0011 SEND_VAL_EX string("Should not happen") 1
0012 DO_FCALL
0013 THROW V7
0014 FAST_RET T6
0015 RETURN int(1)
EXCEPTION TABLE:
     0006, 0006, 0010, 0014
Fatal error: Uncaught Exception: Should happen in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
