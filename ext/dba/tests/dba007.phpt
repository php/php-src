--TEST--
DBA Multiple File Creation Test
--SKIPIF--
<?php 
	require_once('skipif.inc');
	if (!function_exists('dba_list')) die('skip dba_list() not available');
?>
--FILE--
<?php
	require_once('test.inc');
	$db_file1 = dirname(__FILE__).'/test1.dbm'; 
	$db_file2 = dirname(__FILE__).'/test2.dbm'; 
	if (($db_file=dba_open($db_file, "n", $handler))!==FALSE) {
    	echo "database file created with $handler.\n";
	} else {
    	echo "$db_file does not exist\n";
    }
	if (($db_file1=dba_open($db_file1, "n", $handler))!==FALSE) {
    	echo "database file created with $handler.\n";
	} else {
    	echo "$db_file does not exist\n";
    }
	if (($db_file2=dba_open($db_file2, "n", $handler))!==FALSE) {
    	echo "database file created with $handler.\n";
	} else {
    	echo "$db_file does not exist\n";
    }
   	var_dump(dba_list());
	dba_close($db_file);
?>
--EXPECTF--
database file created with %s.
array(3) {
  [%d]=>
  string(%d) "%sext/dba/tests/test.dbm"
  [%d]=>
  string(%d) "%sext/dba/tests/test1.dbm"
  [%d]=>
  string(%d) "%sext/dba/tests/test2.dbm"
}