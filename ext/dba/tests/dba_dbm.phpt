--TEST--
DBA DBM handler test
--SKIPIF--
<?php 
	require_once('skipif.inc');
	if (!in_array('dbm', dba_handlers())) die('skip DBM handler not available');
?>
--FILE--
<?php
	require_once('test.inc');
	$handler = 'dbm';
	require_once('dba_handler.inc');
?>
--EXPECT--
database handler: dbm
3NYNYY
Content String 2
Content 2 replaced
