--TEST--
DBA FlatFile handler test
--SKIPIF--
<?php 
	require_once('skipif.inc');
	if (!in_array('flatfile', dba_handlers())) die('skip FlatFile handler not available');
?>
--FILE--
<?php
	require_once('test.inc');
	$handler = 'flatfile';
	require_once('dba_handler.inc');
?>
--EXPECT--
database handler: flatfile
3NYNYY
Content String 2
Content 2 replaced
