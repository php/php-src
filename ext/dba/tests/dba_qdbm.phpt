--TEST--
DBA QDBM handler test
--SKIPIF--
<?php
	$handler = 'qdbm';
	require_once dirname(__FILE__) .'/skipif.inc';
?>
--FILE--
<?php
	$handler = 'qdbm';
	require_once dirname(__FILE__) .'/test.inc';
	$lock_flag = ''; // lock in library
	require_once dirname(__FILE__) .'/dba_handler.inc';
?>
===DONE===
--EXPECTF--
database handler: qdbm
3NYNYY
Content String 2
Content 2 replaced
Read during write:%sallowed
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

Warning: dba_popen(%stest0.dbm,r-): Locking cannot be disabled for handler qdbm in %sdba_handler.inc on line %d
===DONE===
