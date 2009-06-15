--TEST--
Test posix_getsid() function test
--DESCRIPTION--
Get the current session id of a process pid (POSIX.1, 4.2.1) 
Source code: ext/posix/posix.c
--CREDITS--
Moritz Neuhaeuser, info@xcompile.net
PHP Testfest Berlin 2009-05-10
--SKIPIF--
<?php 
	if(!extension_loaded("posix")) print "SKIP - POSIX extension not loaded"; 
?>
--FILE--
<?php
echo "*** Testing posix_getsid() : function test ***\n";

$pid = posix_getpid();
echo "\n-- Testing posix_getsid() function with current process pid --\n";
var_dump( is_long(posix_getsid($pid)) );

?>
===DONE===
--EXPECTF--
*** Testing posix_getsid() : function test ***

-- Testing posix_getsid() function with current process pid --
bool(true)
===DONE===
