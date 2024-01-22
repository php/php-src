--TEST--
Check LOBS are valid after statement free
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

// Initialization

$stmtarray = array(
    "DROP table lob_041_tab",
    "CREATE table lob_041_tab(c1 CLOB)",
    "INSERT INTO lob_041_tab VALUES('test data')"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Test 1 - explicit statement close\n";

$s = oci_parse($c, "SELECT C1 FROM lob_041_tab");
$desc = oci_new_descriptor($c, OCI_DTYPE_LOB);
oci_define_by_name($s, "C1", $desc);
oci_execute($s);
$data = oci_fetch_assoc($s);
oci_free_statement($s);
echo $data['C1']->load(), "\n";
oci_free_descriptor($desc);

echo "\nTest 2 - implicit statement close\n";

$s = oci_parse($c, "SELECT C1 FROM lob_041_tab");
$desc = oci_new_descriptor($c, OCI_DTYPE_LOB);
oci_define_by_name($s, "C1", $desc);
oci_execute($s);
$data = oci_fetch_assoc($s);
$s = null;
echo $data['C1']->load(), "\n";
oci_free_descriptor($desc);
var_dump($desc);

echo "\nTest 3 - no preallocated descriptor\n";

$s = oci_parse($c, "SELECT C1 FROM lob_041_tab");
oci_execute($s);
$data = oci_fetch_assoc($s);
$s = null;
echo $data['C1']->load(), "\n";
var_dump($data);

// Cleanup

echo "Done\n";

$stmtarray = array(
    "DROP table lob_041_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECTF--
Test 1 - explicit statement close
test data

Test 2 - implicit statement close
test data
object(OCILob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (Unknown)
}

Test 3 - no preallocated descriptor
test data
array(1) {
  ["C1"]=>
  object(OCILob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
Done
