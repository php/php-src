--TEST--
DBA DB4 Multiple File Creation Test
--SKIPIF--
<?php 
$handler = "db4";
require_once(dirname(__FILE__) .'/skipif.inc');
die("info $HND handler used");
?>
--FILE--
<?php
$handler = "db4";
require_once(dirname(__FILE__) .'/test.inc');
echo "database handler: $handler\n";
$db_file1 = $db_filename1 = dirname(__FILE__).'/test1.dbm'; 
$db_file2 = $db_filename2 = dirname(__FILE__).'/test2.dbm'; 
if (($db_file=dba_open($db_file, "n", $handler))!==FALSE) {
    echo "database file created\n";
} else {
    echo "$db_file does not exist\n";
}
if (($db_file1=dba_open($db_file1, "n", $handler))!==FALSE) {
    echo "database file created\n";
} else {
    echo "$db_file does not exist\n";
}
if (($db_file2=dba_open($db_file2, "n", $handler))!==FALSE) {
    echo "database file created\n";
} else {
    echo "$db_file does not exist\n";
}
var_dump(dba_list());
dba_close($db_file);

@unlink($db_filename1);
@unlink($db_filename2);
?>
--CLEAN--
<?php 
require(dirname(__FILE__) .'/clean.inc'); 
?>
--EXPECTF--
database handler: db4
database file created
database file created
database file created
array(3) {
  [%d]=>
  string(%d) "%stest0.dbm"
  [%d]=>
  string(%d) "%stest1.dbm"
  [%d]=>
  string(%d) "%stest2.dbm"
}
