--TEST--
DBA TCADB handler test
--SKIPIF--
<?php 
	$handler = 'tcadb';
	require_once dirname(__FILE__) .'/skipif.inc';
?>
--FILE--
<?php
	$handler = 'tcadb';
    require_once dirname(__FILE__) .'/skipif.inc';
    $lock_flag = 'l';
    $db_filename = $db_file = dirname(__FILE__) .'/test0.tch';
    @unlink($db_filename);
    @unlink($db_filename.'.lck');
	require_once dirname(__FILE__) .'/dba_handler.inc';
?>
===DONE===
--EXPECT--
database handler: tcadb
3NYNYY
Content String 2
Content 2 replaced
Read during write: not allowed
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
--NO-LOCK--
3NYNYY
Content String 2
Content 2 replaced
Read during write: not allowed
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
===DONE===
