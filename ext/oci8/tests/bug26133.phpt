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

$statement = oci_parse($c,$ora_sql);
$rowid = oci_new_descriptor($c,OCI_D_ROWID);
oci_bind_by_name($statement,":v_rowid", $rowid,-1,OCI_B_ROWID);
if (oci_execute($statement)) {
    oci_commit($c);
}
oci_free_statement($statement);
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
