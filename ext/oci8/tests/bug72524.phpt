--TEST--
Bug #72524 (Binding null values triggers ORA-24816 error)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => true);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialize

$stmtarray = array(
         "CREATE TABLE mytable (clob_col CLOB DEFAULT NULL, varchar2_col VARCHAR2(255) DEFAULT NULL)"
);

oci8_test_sql_execute($c, $stmtarray);

// Run test

$sql = "INSERT INTO mytable VALUES (:clob_col, :varchar2_col)";

echo "Test 1 - P1 Value: NULL  P1 Length: Default  P1 Type: Default  P2 Value: NULL P2 Length: Default  P2 Type: Default\n";
$stmt = oci_parse($c, $sql);

$clob = NULL;
$varchar2 = NULL;
oci_bind_by_name($stmt, ':clob_col', $clob);
oci_bind_by_name($stmt, ':varchar2_col', $varchar2);

var_dump(oci_execute($stmt));

echo "Test 2 - P1 Value: ''    P1 Length: Default  P1 Type: Default  P2 Value: ''   P2 Length: Default  P2 Type: Default\n";

$clob = '';
$varchar2 = '';
oci_bind_by_name($stmt, ':clob_col', $clob);
oci_bind_by_name($stmt, ':varchar2_col', $varchar2);

var_dump(oci_execute($stmt));

echo "Test 3 - P1 Value: 'abc' P1 Length: 0        P1 Type: Default  P2 Value: ''   P2 Length: 0        P2 Type: Default\n";
$clob = 'abc';
$varchar2 = 'abc';
oci_bind_by_name($stmt, ':clob_col', $clob, 0);
oci_bind_by_name($stmt, ':varchar2_col', $varchar2, 0);

var_dump(oci_execute($stmt));

echo "Test 4 - P1 Value: NULL  P1 Length: -1       P1 Type: SQLT_LNG P2 Value: NULL P2 Length: -1       P2 Type:Default\n";
$clob = NULL;
$varchar2 = NULL;
oci_bind_by_name($stmt, ':clob_col', $clob, -1, SQLT_LNG);
oci_bind_by_name($stmt, ':varchar2_col', $varchar2, -1, SQLT_LNG);

var_dump(oci_execute($stmt));

echo "Test 5 - P1 Value: NULL  P1 Length: 0        P1 Type: SQLT_LNG P2 Value: NULL P2 Length: 0        P2 Type:Default\n";
$clob = NULL;
$varchar2 = NULL;
oci_bind_by_name($stmt, ':clob_col', $clob, 0, SQLT_LNG);
oci_bind_by_name($stmt, ':varchar2_col', $varchar2, 0, SQLT_LNG);


var_dump(oci_execute($stmt));

// Cleanup

$stmtarray = array(
    "DROP TABLE mytable"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1 - P1 Value: NULL  P1 Length: Default  P1 Type: Default  P2 Value: NULL P2 Length: Default  P2 Type: Default
bool(true)
Test 2 - P1 Value: ''    P1 Length: Default  P1 Type: Default  P2 Value: ''   P2 Length: Default  P2 Type: Default
bool(true)
Test 3 - P1 Value: 'abc' P1 Length: 0        P1 Type: Default  P2 Value: ''   P2 Length: 0        P2 Type: Default
bool(true)
Test 4 - P1 Value: NULL  P1 Length: -1       P1 Type: SQLT_LNG P2 Value: NULL P2 Length: -1       P2 Type:Default

Warning: oci_execute(): ORA-24816: %s in %s on line %d
bool(false)
Test 5 - P1 Value: NULL  P1 Length: 0        P1 Type: SQLT_LNG P2 Value: NULL P2 Length: 0        P2 Type:Default

Warning: oci_execute(): ORA-24816: %s in %s on line %d
bool(false)
===DONE===
