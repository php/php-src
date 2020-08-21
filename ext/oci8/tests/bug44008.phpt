--TEST--
Bug #44008 (Incorrect usage of OCILob->close crashes PHP)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

// Initialization

$stmtarray = array(
        "create or replace procedure bug44008_proc (p in out clob)
        as begin p := 'A';
        end;"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

$s = oci_parse($c, 'begin bug44008_proc(:data); end;');
$textLob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ":data", $textLob, -1, OCI_B_CLOB);
oci_execute($s, OCI_DEFAULT);
$r = $textLob->load();
echo "$r\n";

// Incorrectly closing the lob doesn't cause a crash.
// OCI-LOB->close() is documented for use only with OCILob->writeTemporary()
$textLob->close();

// Cleanup

$stmtarray = array(
        "drop procedure bug44008_proc"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECT--
A
Done
