--TEST--
Bug #37220 (LOB Type mismatch when using windows & oci8.dll)
--EXTENSIONS--
oci8
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
    "create table bug37220_tab( mycolumn xmltype not null)",
    "insert into bug37220_tab values(xmltype('<THETAG myID=\"1234\"></THETAG>'))"
);

oci8_test_sql_execute($c, $stmtarray);

// Now let's update the row where myId = 1234 and change the tag
// 'THETAG' to 'MYTAG' (mycolumn is an XMLTYPE datatype and
// bug37220_tab a normal Oracle table)

$query = "UPDATE  bug37220_tab
          SET     bug37220_tab.mycolumn = updateXML(bug37220_tab.mycolumn,'/THETAG',xmltype.createXML(:data))
          WHERE   existsNode(bug37220_tab.mycolumn,'/THETAG[@myID=\"1234\"]') = 1";
$stmt = oci_parse ($c, $query);
$clob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($stmt, ':data', $clob, -1, OCI_B_CLOB);
$clob->writeTemporary("<MYTAG/>", OCI_TEMP_CLOB);
$success = oci_execute($stmt, OCI_COMMIT_ON_SUCCESS);
oci_free_statement($stmt);
$clob->close();

// Query back the change

$query = "select * from bug37220_tab";
$stmt = oci_parse ($c, $query);

oci_execute($stmt);

while ($row = oci_fetch_array($stmt, OCI_ASSOC+OCI_RETURN_NULLS)) {
    foreach ($row as $item) {
        echo trim($item)."\n";
    }
    echo "\n";
}

// Cleanup

$stmtarray = array(
    "drop table bug37220_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECT--
<MYTAG/>

Done
