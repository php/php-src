--TEST--
DBA GDBM handler test
--SKIPIF--
<?php
    $handler = 'gdbm';
    require_once __DIR__ .'/skipif.inc';
?>
--FILE--
<?php
    $handler = 'gdbm';
    require_once __DIR__ .'/test.inc';
    $lock_flag = ''; // lock in library
    require_once __DIR__ .'/dba_handler.inc';

    // Read during write is system dependent. Important is that there is no deadlock
?>
--EXPECTF--
database handler: gdbm
3NYNYY
Content String 2
Content 2 replaced
Read during write:%sallowed
"key number 6" written
Failed to write "key number 6" 2nd time
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
