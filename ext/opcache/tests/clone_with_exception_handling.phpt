--TEST--
Opcache does not eliminate try-catch block around clone with
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php

class Foo
{
    public int $bar = 1;
}

$obj1 = new Foo();

try {
    clone $obj1 with ["bar" => []];
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
$_main:
     ; %s
     ; (after optimizer)
     ; %s
0000 V2 = NEW 0 string("Foo")
0001 DO_FCALL
0002 ASSIGN CV0($obj1) V2
0003 V2 = CLONE CV0($obj1)
0004 CLONE_INIT_PROP V2 string("bar")
0005 OP_DATA array(...)
0006 RETURN int(1)
0007 CV1($e) = CATCH string("TypeError")
0008 INIT_METHOD_CALL 0 CV1($e) string("getMessage")
0009 V3 = DO_FCALL
0010 T2 = CONCAT V3 string("
")
0011 ECHO T2
0012 RETURN int(1)
LIVE RANGES:
     2: 0001 - 0002 (new)
EXCEPTION TABLE:
     0003, 0007, -, -
Cannot assign array to property Foo::$bar of type int
