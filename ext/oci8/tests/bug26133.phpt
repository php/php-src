--TEST--
Bug #26133 (ocifreedesc() segfault)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialize

$stmtarray = array(
    "drop table bug26133_tab",
    "create table bug26133_tab (id number, value number)",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

$ora_sql = "INSERT INTO bug26133_tab (id, value) VALUES (1,'1') RETURNING ROWID INTO :v_rowid ";

$statement = OCIParse($c,$ora_sql);
$rowid = OCINewDescriptor($c,OCI_D_ROWID);
OCIBindByName($statement,":v_rowid", $rowid,-1,OCI_B_ROWID);
if (OCIExecute($statement)) {
    OCICommit($c);
}
OCIFreeStatement($statement);
$rowid->free();

// Cleanup

$stmtarray = array(
    "drop table bug26133_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";
?>
--EXPECT--
Done
