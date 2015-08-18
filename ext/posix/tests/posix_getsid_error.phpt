--TEST--
Test posix_getsid() function : error conditions 
--DESCRIPTION--
cases: no params, wrong param, wrong param range
--CREDITS--
Moritz Neuhaeuser, info@xcompile.net
PHP Testfest Berlin 2009-05-10
--SKIPIF--
<?php 
	if(!extension_loaded("posix")) {
        die("SKIP - POSIX extension not loaded"); 
    }
?>
--FILE--
<?php
var_dump( posix_getsid() );
var_dump( posix_getsid(array()) );
var_dump( posix_getsid(-1) );
?>
===DONE===
--EXPECTF--
Warning: posix_getsid() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

Warning: posix_getsid() expects parameter 1 to be integer, array given in %s on line %d
bool(false)
bool(false)
===DONE===
