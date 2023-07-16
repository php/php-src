--TEST--
GH-11245: In some specific cases SWITCH with one default statement will cause segfault (VAR variation)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0x7FFFBFFF
opcache.opt_debug_level=0x20000
opcache.preload=
--EXTENSIONS--
opcache
--FILE--
<?php
function xx() { return "somegarbage"; }
switch (xx()) {
	default:
		if (!empty($xx)) {return;}
}
?>
--EXPECTF--
$_main:
     ; (lines=4, args=0, vars=1, tmps=1)
     ; (after optimizer)
     ; %s
0000 T1 = ISSET_ISEMPTY_CV (empty) CV0($xx)
0001 JMPNZ T1 0003
0002 RETURN null
0003 RETURN int(1)

xx:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN string("somegarbage")
