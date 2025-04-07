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
0001 JMPNZ T1 0006
0002 V3 = NEW 1 string("Exception")
0003 SEND_VAL_EX string("Should happen") 1
0004 DO_FCALL
0005 THROW V3
0006 JMP 0006
0007 V6 = NEW 1 string("Exception")
0008 SEND_VAL_EX string("Should not happen") 1
0009 DO_FCALL
0010 THROW V6
0011 FAST_RET T5
EXCEPTION TABLE:
     0006, -, 0007, 0011
Fatal error: Uncaught Exception: Should happen in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
