--TEST--
Nullsafe basic optimization
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
zend_test.observer.enabled=0
--EXTENSIONS--
opcache
--FILE--
<?php

function test() {
    $null = null;
    var_dump($null?->foo);
    var_dump(isset($null?->foo));
    var_dump(empty($null?->foo));
}

function test2(object $obj) {
    // TODO: Optimize the JMP_NULL away.
    var_dump($obj?->foo);
    var_dump(isset($obj?->foo));
    var_dump(empty($obj?->foo));
}

?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(1)

test:
     ; (lines=10, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 INIT_FCALL 1 %d string("var_dump")
0001 SEND_VAL null 1
0002 DO_ICALL
0003 INIT_FCALL 1 %d string("var_dump")
0004 SEND_VAL bool(false) 1
0005 DO_ICALL
0006 INIT_FCALL 1 %d string("var_dump")
0007 SEND_VAL bool(true) 1
0008 DO_ICALL
0009 RETURN null

test2:
     ; (lines=17, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %s
0000 CV0($obj) = RECV 1
0001 INIT_FCALL 1 %d string("var_dump")
0002 T1 = JMP_NULL CV0($obj) 0004
0003 T1 = FETCH_OBJ_R CV0($obj) string("foo")
0004 SEND_VAL T1 1
0005 DO_ICALL
0006 INIT_FCALL 1 %d string("var_dump")
0007 T1 = JMP_NULL CV0($obj) 0009
0008 T1 = ISSET_ISEMPTY_PROP_OBJ (isset) CV0($obj) string("foo")
0009 SEND_VAL T1 1
0010 DO_ICALL
0011 INIT_FCALL 1 %d string("var_dump")
0012 T1 = JMP_NULL CV0($obj) 0014
0013 T1 = ISSET_ISEMPTY_PROP_OBJ (empty) CV0($obj) string("foo")
0014 SEND_VAL T1 1
0015 DO_ICALL
0016 RETURN null
