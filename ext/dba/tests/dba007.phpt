--TEST--
DBA Multiple File Creation Test
--SKIPIF--
<?php
    require_once(__DIR__ .'/skipif.inc');
    if (!function_exists('dba_list')) die('skip dba_list() not available');
    die("info $HND handler used");
?>
--FILE--
<?php
    require_once(__DIR__ .'/test.inc');
    echo "database handler: $handler\n";
    $db_file1 = $db_filename1 = __DIR__.'/test1.dbm';
    $db_file2 = $db_filename2 = __DIR__.'/test2.dbm';
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
    require(__DIR__ .'/clean.inc');
?>
--EXPECTF--
database handler: %s
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
