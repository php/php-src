--TEST--
DBA DB2 handler test
--SKIPIF--
<?php 
	require_once('skipif.inc');
	if (!in_array('db2', dba_handlers())) die('skip DB2 handler not available');
?>
--FILE--
<?php
	require_once('test.inc');
	$handler = 'db2';
	require_once('dba_handler.inc');
?>
--EXPECT--
database handler: db2
3NYNYY
Content String 2
Content 2 replaced
