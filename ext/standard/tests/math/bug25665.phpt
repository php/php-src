--TEST--
Bug #25665 (var_dump () hangs on Nan and INF)
--SKIPIF--
<?php 
	$OS = strtoupper(PHP_OS);
	if ($OS == 'SUNOS' || $OS == 'SOLARIS') die("SKIP Solaris acos() returns wrong value");
?>
--FILE--
<?php
set_time_limit(5);
var_dump(acos(1.01));
var_dump(log(0));
?>
--EXPECT--
float(NAN)
float(-INF)
