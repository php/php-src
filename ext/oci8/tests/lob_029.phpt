--TEST--
reading/writing BFILE LOBs
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

$realdirname = dirname(__FILE__);
$realfilename = "oci8bfiletest.txt";
$fullname = $realdirname."/".$realfilename;

// Setup
$s = oci_parse($c, "create directory TestDir as '$realdirname'");
oci_execute($s);

file_put_contents($fullname, 'Some text in the bfile');

$s = oci_parse($c, "create table FileTest (FileNum number, FileDesc varchar2(30), Image bfile)");
oci_execute($s);

$s = oci_parse($c, "insert into FileTest (FileNum, FileDesc, Image) values (1, 'Description 1', bfilename('TESTDIR', '$realfilename'))");
oci_execute($s);

// Run tests

echo "Test 1. Check how many rows in the table\n";

$s = oci_parse($c, "select count(*) numrows from FileTest");
oci_execute($s);
oci_fetch_all($s, $res);
var_dump($res);

echo "Test 2\n";
$s = oci_parse($c, "select * from FileTest");
oci_execute($s);
oci_fetch_all($s, $res);
var_dump($res);

echo "Test 3\n";
$d = oci_new_descriptor($c, OCI_D_FILE);

$s = oci_parse($c, "insert into FileTest (FileNum, FileDesc, Image) values (2, 'Description 2', bfilename('TESTDIR', '$realfilename')) returning Image into :im");
oci_bind_by_name($s, ":im", $d, -1, OCI_B_BFILE);
oci_execute($s);

$r = $d->read(40);
var_dump($r);

unlink($fullname);

$s = oci_parse($c, "drop table FileTest");
oci_execute($s);

$s = oci_parse($c, "drop directory TestDir");
oci_execute($s);

echo "Done\n";
?>
--EXPECTF--	
Test 1. Check how many rows in the table
array(1) {
  ["NUMROWS"]=>
  array(1) {
    [0]=>
    string(1) "1"
  }
}
Test 2
array(3) {
  ["FILENUM"]=>
  array(1) {
    [0]=>
    string(1) "1"
  }
  ["FILEDESC"]=>
  array(1) {
    [0]=>
    string(13) "Description 1"
  }
  ["IMAGE"]=>
  array(1) {
    [0]=>
    string(22) "Some text in the bfile"
  }
}
Test 3
string(22) "Some text in the bfile"
Done
--UEXPECTF--
Test 1. Check how many rows in the table
array(1) {
  [u"NUMROWS"]=>
  array(1) {
    [0]=>
    unicode(1) "1"
  }
}
Test 2
array(3) {
  [u"FILENUM"]=>
  array(1) {
    [0]=>
    unicode(1) "1"
  }
  [u"FILEDESC"]=>
  array(1) {
    [0]=>
    unicode(13) "Description 1"
  }
  [u"IMAGE"]=>
  array(1) {
    [0]=>
    string(22) "Some text in the bfile"
  }
}
Test 3
string(22) "Some text in the bfile"
Done
