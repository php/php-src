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
    while (true) { }
} finally {
    throw new Exception("Should not happen");
}

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=0, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 T1 = ISSET_ISEMPTY_CV (isset) CV0($badvar)
0001 JMPNZ T1 0007
0002 T3 = NEW 1 string("Exception")
0003 SEND_VAL%S string("Should happen") 1
0004 DO_FCALL
0005 FLUSH_DEFERRED_DTORS
0006 THROW T3
0007 FLUSH_DEFERRED_DTORS
0008 JMP 0008
0009 T6 = NEW 1 string("Exception")
0010 SEND_VAL%S string("Should not happen") 1
0011 DO_FCALL
0012 FLUSH_DEFERRED_DTORS
0013 THROW T6
0014 FAST_RET T5
EXCEPTION TABLE:
     0008, -, 0009, 0014

Fatal error: Uncaught Exception: Should happen in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
