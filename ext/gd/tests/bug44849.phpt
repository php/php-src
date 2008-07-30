--TEST--
Bug #44849 (imagecolorclosesthwb is not available on Windows)
--SKIPIF--
<?php
	if(!extension_loaded('gd')){ die('skip: gd extension not available'); }
	if(strtoupper(substr(PHP_OS, 0, 3)) != 'WIN'){ die('skip: not a Windows operating system'); }
?>
--FILE--
<?php
	var_dump(function_exists('imagecolorclosesthwb'));
?>
--EXPECTF--
bool(true)
