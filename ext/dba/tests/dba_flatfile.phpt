--TEST--
DBA FlatFile handler test
--SKIPIF--
<?php 
	$handler = 'flatfile';
	require_once(dirname(__FILE__) .'/skipif.inc');
?>
--FILE--
<?php
	$handler = 'flatfile';
	require_once(dirname(__FILE__) .'/test.inc');
	require_once(dirname(__FILE__) .'/dba_handler.inc');
?>
===DONE===
--CLEAN--
<?php 
	require(dirname(__FILE__) .'/clean.inc'); 
?>
--EXPECT--
database handler: flatfile
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
