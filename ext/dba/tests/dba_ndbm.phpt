--TEST--
DBA NDBM handler test
--SKIPIF--
<?php 
	require_once('skipif.inc');
	if (!in_array('ndbm', dba_handlers())) die('skip NDBM handler not available');
?>
--FILE--
<?php
	require_once('test.inc');
	$handler = 'ndbm';
	require_once('dba_handler.inc');
?>
--EXPECT--
database handler: ndbm
3NYNYY
Content String 2
Content 2 replaced
