--TEST--
DBA DB3 handler test
--SKIPIF--
<?php 
	require_once('skipif.inc');
	if (!in_array('db3', dba_handlers())) die('skip DB3 handler not available');
?>
--FILE--
<?php
	require_once('test.inc');
	$handler = 'db3';
	require_once('dba_handler.inc');
?>
--EXPECT--
database handler: db3
3NYNYY
Content String 2
Content 2 replaced
