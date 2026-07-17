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
0001 JMPNZ T2 0007
0002 T4 = NEW 1 string("Exception")
0003 SEND_VAL%S string("Should happen") 1
0004 DO_FCALL
0005 FLUSH_DEFERRED_DTORS
0006 THROW T4
0007 FLUSH_DEFERRED_DTORS
0008 FLUSH_DEFERRED_DTORS
0009 JMP 0012
0010 CV1($e) = CATCH string("Throwable")
0011 ECHO string("foo")
0012 T6 = FAST_CALL 0014
0013 JMP 0020
0014 T7 = NEW 1 string("Exception")
0015 SEND_VAL%S string("Should not happen") 1
0016 DO_FCALL
0017 FLUSH_DEFERRED_DTORS
0018 THROW T7
0019 FAST_RET T6
0020 RETURN int(1)
EXCEPTION TABLE:
     0008, 0010, 0014, 0019

Fatal error: Uncaught Exception: Should happen in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
