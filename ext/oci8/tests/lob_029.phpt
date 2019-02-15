--TEST--
reading/writing BFILE LOBs
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
ob_start();
phpinfo(INFO_MODULES);
$phpinfo = ob_get_clean();
if (preg_match('/Compile-time ORACLE_HOME/', $phpinfo) !== 1) {
    // Assume building PHP with an ORACLE_HOME means the tested DB is on the same machine as PHP
    die("skip this test won't work with remote Oracle");
}
if (substr(PHP_OS, 0, 3) == 'WIN') die("skip Test script not ported to Windows");
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

$realdirname = "/tmp";  // Use /tmp because a local dir can give ORA-22288 depending on perms
$realfilename1 = "oci8bfiletest1.txt";
$fullname1 = $realdirname."/".$realfilename1;
$realfilename2 = "oci8bfiletest2.txt";
$fullname2 = $realdirname."/".$realfilename2;
$realfilename3 = "oci8bfiletest3.txt";
$fullname3 = $realdirname."/".$realfilename3;

// Setup
$s = oci_parse($c, "drop table FileTest");
@oci_execute($s);

$s = oci_parse($c, "drop directory TestDir");
@oci_execute($s);

$s = oci_parse($c, "create directory TestDir as '$realdirname'");
oci_execute($s);

file_put_contents($fullname1, 'Some text in the bfile 1');
file_put_contents($fullname2, 'Some text in the bfile 2');
file_put_contents($fullname3, 'Some text in the bfile 3');

$s = oci_parse($c, "create table FileTest (FileNum number, FileDesc varchar2(30), Image bfile)");
oci_execute($s);

$s = oci_parse($c, "insert into FileTest (FileNum, FileDesc, Image) values (1, 'Description 1', bfilename('TESTDIR', '$realfilename1'))");
oci_execute($s);

$s = oci_parse($c, "insert into FileTest (FileNum, FileDesc, Image) values (2, 'Description 2', bfilename('TESTDIR', '$realfilename2'))");
oci_execute($s);

$s = oci_parse($c, "insert into FileTest (FileNum, FileDesc, Image) values (3, 'Description 3', bfilename('TESTDIR', '$realfilename3'))");
oci_execute($s);

// Run tests

echo "Test 1. Check how many rows in the table\n";

$s = oci_parse($c, "select count(*) numrows from FileTest");
oci_execute($s);
oci_fetch_all($s, $res);
var_dump($res);

echo "Test 2\n";
$s = oci_parse($c, "select * from FileTest order by FileNum");
oci_execute($s);
oci_fetch_all($s, $res);
var_dump($res);

echo "Test 3\n";
$d = oci_new_descriptor($c, OCI_D_FILE);

$s = oci_parse($c, "insert into FileTest (FileNum, FileDesc, Image) values (2, 'Description 2', bfilename('TESTDIR', '$realfilename1')) returning Image into :im");
oci_bind_by_name($s, ":im", $d, -1, OCI_B_BFILE);
oci_execute($s);

$r = $d->read(40);
var_dump($r);

unlink($fullname1);
unlink($fullname2);
unlink($fullname3);

$s = oci_parse($c, "drop table FileTest");
oci_execute($s);

$s = oci_parse($c, "drop directory TestDir");
oci_execute($s);

echo "Done\n";
?>
--EXPECT--
Test 1. Check how many rows in the table
array(1) {
  ["NUMROWS"]=>
  array(1) {
    [0]=>
    string(1) "3"
  }
}
Test 2
array(3) {
  ["FILENUM"]=>
  array(3) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    [2]=>
    string(1) "3"
  }
  ["FILEDESC"]=>
  array(3) {
    [0]=>
    string(13) "Description 1"
    [1]=>
    string(13) "Description 2"
    [2]=>
    string(13) "Description 3"
  }
  ["IMAGE"]=>
  array(3) {
    [0]=>
    string(24) "Some text in the bfile 1"
    [1]=>
    string(24) "Some text in the bfile 2"
    [2]=>
    string(24) "Some text in the bfile 3"
  }
}
Test 3
string(24) "Some text in the bfile 1"
Done
