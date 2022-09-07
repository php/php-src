--TEST--
Clone with property initialization
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php

function test() {
    $obj = new stdClass();

    return clone $obj with {getPropertyName("a") => 2, b: new stdClass()};
}

function getPropertyName($prop) {
    return $prop;
}

?>
--EXPECTF--
$_main:
     ; %s
     ; (after optimizer)
     ; %s
0000 RETURN int(1)

test:
     ; %s
     ; (after optimizer)
     ; %s
0000 V1 = NEW 0 string("stdClass")
0001 DO_FCALL
0002 CV0($obj) = QM_ASSIGN V1
0003 V1 = CLONE CV0($obj)
0004 INIT_FCALL 1 96 string("getpropertyname")
0005 SEND_VAL string("a") 1
0006 V2 = DO_UCALL
0007 T3 = CLONE_INIT_PROP V1 V2
0008 OP_DATA int(2)
0009 V2 = NEW 0 string("stdClass")
0010 DO_FCALL
0011 V1 = CLONE_INIT_PROP T3 string("b")
0012 OP_DATA V2
0013 RETURN V1
LIVE RANGES:
%s
%s
%s
%s
%s

getPropertyName:
     ; %s
     ; (after optimizer)
     ; %s
0000 CV0($prop) = RECV 1
0001 RETURN CV0($prop)
