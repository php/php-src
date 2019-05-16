--TEST--
Test some more oci_bind_by_name error conditions
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
$target_dbs = array('oracledb' => true, 'timesten' => true);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) && $matches[1] >= 12)) {
    die("skip expected output only valid when using Oracle Database 12c or greater");
}
preg_match('/^[[:digit:]]+/', oci_client_version(), $matches);
if (!(isset($matches[0]) && $matches[0] >= 12)) {
    die("skip works only with Oracle 12c or greater version of Oracle client libraries");
}
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "drop table error_bind_2_tab",
    "create table error_bind_2_tab(name varchar(10))"
);
oci8_test_sql_execute($c, $stmtarray);

echo "Test 1 - SQLT_BOL\n";

unset($name);
$stmt = oci_parse($c, "insert into error_bind_2_tab values (:name)");
oci_bind_by_name($stmt, ":name", $name, -1, SQLT_BOL);
$name=$c;
var_dump(oci_execute($stmt));

echo "Test 2 - SQLT_BOL\n";

unset($name);
$stmt = oci_parse($c, "insert into error_bind_2_tab values (:name)");
$name=$c;
oci_bind_by_name($stmt, ":name", $name, -1, SQLT_BOL);

// Clean up

$stmtarray = array(
    "drop table error_bind_2_tab",
);
oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECTF--
Test 1 - SQLT_BOL

Warning: oci_execute(): Invalid variable used for bind in %s on line %d
bool(false)
Test 2 - SQLT_BOL

Warning: oci_bind_by_name(): Invalid variable used for bind in %s on line %d
Done
