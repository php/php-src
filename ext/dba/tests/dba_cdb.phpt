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