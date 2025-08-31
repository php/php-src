--TEST--
GH-16990 (dba_list() is now zero-indexed instead of using resource ids)
--EXTENSIONS--
dba
--CONFLICTS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('flatfile');
?>
--FILE--
<?php

require_once(__DIR__ .'/test.inc');

$foo1 = new stdClass();

$db_filename1 = __DIR__.'/test1.dbm';
$db1 = dba_open($db_filename1, 'c', 'flatfile');
if (!$db1) {
    var_dump("Database file creation failed");
}

$foo2 = new stdClass();

$db_filename2 = __DIR__.'/test2.dbm';
$db2 = dba_open($db_filename2, 'c', 'flatfile');
if (!$db2) {
    var_dump("Database file creation failed");
}

var_dump(dba_list());
?>
--CLEAN--
<?php
@unlink(__DIR__.'/test1.dbm');
@unlink(__DIR__.'/test2.dbm');
?>
--EXPECTF--
array(2) {
  [2]=>
  string(%d) "%s%etest1.dbm"
  [4]=>
  string(%d) "%s%etest2.dbm"
}
