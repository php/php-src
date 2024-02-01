--TEST--
DBA DB4 Multiple File Creation Test
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('db4');
?>
--FILE--
<?php
$handler = "db4";
echo "database handler: $handler\n";
$db_file1 = __DIR__.'/test1.dbm';
$db_file2 = __DIR__.'/test2.dbm';
$db_file1 = dba_open($db_file1, "n", $handler);
$db_file2 = dba_open($db_file2, "n", $handler);
var_dump(dba_list());
dba_close($db_file1);
dba_close($db_file2);

?>
--CLEAN--
<?php
$db_filename1 = __DIR__.'/test1.dbm';
$db_filename2 = __DIR__.'/test2.dbm';
@unlink($db_filename1);
@unlink($db_filename2);
?>
--EXPECTF--
database handler: db4
array(2) {
  [%d]=>
  string(%d) "%stest1.dbm"
  [%d]=>
  string(%d) "%stest2.dbm"
}
