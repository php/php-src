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

clone new stdClass() with ["x" => 1, "y" => 2, "z" => 3];

?>
===DONE===
--EXPECTF--
$_main:
     ; %s
     ; (after optimizer)
     ; %s
0000 V1 = NEW 0 string("stdClass")
0001 DO_FCALL
0002 V0 = CLONE V1
0003 T1 = CLONE_INIT_PROP V0 string("x")
0004 OP_DATA int(1)
0005 T0 = CLONE_INIT_PROP T1 string("y")
0006 OP_DATA int(2)
0007 CLONE_INIT_PROP T0 string("z")
0008 OP_DATA int(3)
0009 ECHO string("===DONE===
")
0010 RETURN int(1)
LIVE RANGES:
%s
%s
%s
===DONE===
