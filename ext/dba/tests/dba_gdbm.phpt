--TEST--
DBA GDBM handler test
--SKIPIF--
<?php 
	require_once('skipif.inc');
	if (!in_array('gdbm', dba_handlers())) die('skip GDBM handler not available');
?>
--FILE--
<?php
	require_once('test.inc');
	$handler = 'gdbm';
	require_once('dba_handler.inc');
?>
--EXPECT--
database handler: gdbm
3NYNYY
Content String 2
Content 2 replaced
