--TEST--
Test oci_define_by_name() LOB descriptor
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

$stmtarray = array(
    "drop table phpdefblobtable",
    "create table phpdefblobtable (id number(10), fileimage blob)"
);

oci8_test_sql_execute($c, $stmtarray);

// Load data
$stmt = oci_parse ($c, "insert into phpdefblobtable (id, fileimage) values (:id, empty_blob()) returning fileimage into :fileimage");
$fileimage = oci_new_descriptor($c,OCI_D_LOB);
oci_bind_by_name($stmt,":id",$id);
oci_bind_by_name($stmt,":fileimage",$fileimage,-1,OCI_B_BLOB);
$id = 1;
oci_execute($stmt, OCI_DEFAULT);
$fileimage->saveFile(__DIR__."/test.gif");
$data = $fileimage->load();
var_dump(md5($data));  // original md5
oci_commit($c);

// New row with different data
$id = 2;
$data = strrev($data);
var_dump(md5($data));
oci_execute($stmt, OCI_DEFAULT);
$fileimage->save($data);
oci_commit($c);

echo "Test 1\n";
$stmt = oci_parse($c, "SELECT fileimage FROM phpdefblobtable");
var_dump(oci_define_by_name($stmt, 'FILEIMAGE', $f));
oci_execute($stmt);

while (oci_fetch($stmt)) {
   var_dump($f);
   echo "file md5:" . md5($f->load()) . "\n";
}

echo "Test 2\n";
$stmt = oci_parse($c, "SELECT fileimage FROM phpdefblobtable");
var_dump(oci_define_by_name($stmt, 'FILEIMAGE', $outdata, SQLT_STR));
oci_execute($stmt);

while (oci_fetch($stmt)) {
   echo "file md5:" . md5($outdata) . "\n";
}

echo "Test 3\n";
$stmt = oci_parse($c, "SELECT fileimage FROM phpdefblobtable");
var_dump(oci_define_by_name($stmt, 'FILEIMAGE', $outdata, SQLT_BIN));
oci_execute($stmt);

while (oci_fetch($stmt)) {
   echo "file md5:" . md5($outdata) . "\n";
}

echo "Test 4\n";
$fid = oci_new_descriptor($c,OCI_D_LOB);
$stmt = oci_parse($c, "SELECT fileimage FROM phpdefblobtable");
var_dump(oci_define_by_name($stmt, 'FILEIMAGE', $fid));
oci_execute($stmt);

while (oci_fetch($stmt)) {
   echo "file md5:" . md5($fid->load()) . "\n";
}

$stmtarray = array(
    "drop table phpdefblobtable"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECTF--
string(32) "614fcbba1effb7caa27ef0ef25c27fcf"
string(32) "06d4f219d946c74d748d43932cd9dcb2"
Test 1
bool(true)
object(OCILob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
file md5:614fcbba1effb7caa27ef0ef25c27fcf
object(OCILob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
file md5:06d4f219d946c74d748d43932cd9dcb2
Test 2
bool(true)

Warning: oci_fetch(): ORA-00932: %s on line %d
Test 3
bool(true)
file md5:614fcbba1effb7caa27ef0ef25c27fcf
file md5:06d4f219d946c74d748d43932cd9dcb2
Test 4
bool(true)
file md5:614fcbba1effb7caa27ef0ef25c27fcf
file md5:06d4f219d946c74d748d43932cd9dcb2
Done
