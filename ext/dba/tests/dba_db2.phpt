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
Read during write permitted
Content 2 replaced 2nd time
The 6th value
array(3) {
  ["key number 6"]=>
  string(13) "The 6th value"
  ["key2"]=>
  string(27) "Content 2 replaced 2nd time"
  ["key5"]=>
  string(23) "The last content string"
}