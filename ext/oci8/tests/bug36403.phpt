--TEST--
Bug #36403 (oci_execute no longer supports OCI_DESCRIBE_ONLY)
--EXTENSIONS--
oci8
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "drop table bug36403_tab",
    "create table bug36403_tab (c1 number, col2 number, column3 number, col4 number)"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

echo "Test 1\n";

$s = oci_parse($c, "select * from bug36403_tab");
oci_execute($s, OCI_DESCRIBE_ONLY);
for ($i = oci_num_fields($s); $i > 0; $i--) {
    echo oci_field_name($s, $i) . "\n";
}

echo "Test 2\n";

// Should generate an error: ORA-24338: statement handle not executed
// since the statement handle was only described and not executed
$row = oci_fetch_array($s);

// Clean up

$stmtarray = array(
    "drop table bug36403_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECTF--
Test 1
COL4
COLUMN3
COL2
C1
Test 2

Warning: oci_fetch_array(): ORA-%r(24338|01002)%r: %sbug36403.php on line %d
