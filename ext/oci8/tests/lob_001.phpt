--TEST--
oci_lob_write() and friends
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "drop table lob_001_tab",
    "create table lob_001_tab (id number, b1 blob)",
);

oci8_test_sql_execute($c, $stmtarray);

echo "Test 1 OCI_B_BLOB bind\n";

$statement = oci_parse($c, "insert into lob_001_tab (id, b1) values (1, empty_blob()) returning b1 into :v_b1 ");
$blob = oci_new_descriptor($c,OCI_D_LOB);
var_dump(oci_bind_by_name($statement, ":v_b1", $blob, -1, OCI_B_BLOB));
oci_execute($statement, OCI_DEFAULT);
var_dump($blob);

echo "Test 2\n";

var_dump($blob->write("test"));
var_dump($blob->tell());
var_dump($blob->seek(10, OCI_SEEK_CUR));
var_dump($blob->write("string"));
var_dump($blob->flush());

oci_commit($c);

echo "Test 3\n";

$s = oci_parse($c, "select b1 from lob_001_tab where id = 1");
oci_execute($s);
var_dump(oci_fetch_array($s, OCI_RETURN_LOBS));

echo "Test 4 SQLT_BLOB (an alias for OCI_B_BLOB) bind\n";

$statement = oci_parse($c, "insert into lob_001_tab (id, b1) values (2, empty_blob()) returning b1 into :v_b1 ");
$blob = oci_new_descriptor($c,OCI_D_LOB);
var_dump(oci_bind_by_name($statement, ":v_b1", $blob, -1, SQLT_BLOB));
oci_execute($statement, OCI_DEFAULT);
var_dump($blob->write("test row 2"));

$s = oci_parse($c, "select b1 from lob_001_tab where id = 2");
oci_execute($s);
var_dump(oci_fetch_array($s, OCI_RETURN_LOBS));

// Cleanup

$stmtarray = array(
    "drop table lob_001_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECTF--
Test 1 OCI_B_BLOB bind
bool(true)
object(OCILob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
Test 2
int(4)
int(4)
bool(true)
int(6)
bool(false)
Test 3
array(2) {
  [0]=>
  string(20) "test%0%0%0%0%0%0%0%0%0%0string"
  ["B1"]=>
  string(20) "test%0%0%0%0%0%0%0%0%0%0string"
}
Test 4 SQLT_BLOB (an alias for OCI_B_BLOB) bind
bool(true)
int(10)
array(2) {
  [0]=>
  string(10) "test row 2"
  ["B1"]=>
  string(10) "test row 2"
}
