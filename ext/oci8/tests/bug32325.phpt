--TEST--
Bug #32325 (Cannot retrieve collection using OCI8)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialize

$stmtarray = array(
    "create or replace type bug32325_t as table of number"
);

oci8_test_sql_execute($c, $stmtarray);

// Run test

$collection = oci_new_collection($c, "BUG32325_T");

$sql = "begin
        select bug32325_t(1,2,3,4) into :list from dual;
        end;";

$stmt = oci_parse($c, $sql);

oci_bind_by_name($stmt, ":list",  $collection, -1, OCI_B_NTY);
oci_execute($stmt);

var_dump($collection->size());
var_dump($collection->getelem(1));
var_dump($collection->getelem(2));

// Cleanup

$stmtarray = array(
    "drop type bug32325_t"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";
?>
--EXPECT--
int(4)
float(2)
float(3)
Done
