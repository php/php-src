--TEST--
DBA CDB handler test
--SKIPIF--
<?php 
	require_once('skipif.inc');
	if (!in_array('cdb', dba_handlers())) die('skip CDB handler not available');
	die('skip CDB does not support replace or delete');
?>
--FILE--
<?php
	require_once('test.inc');
	$handler = 'cdb';
	require_once('dba_handler.inc');
?>
--EXPECT--
database handler: cdb
3NYNYY
Content String 2
Content 2 replaced
