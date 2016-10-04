--TEST--
string zlib_get_coding_type ( void );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
if(!extension_loaded('zlib')) die('skip, zlib not loaded');
?>
--FILE--
<?php
var_dump(zlib_get_coding_type());
?>
--EXPECTF--
bool(%s)
