--TEST--
binding a cursor (with errors)
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table cursor_bind_err_tab",
    "create table cursor_bind_err_tab (id number, value number)",
    "insert into cursor_bind_err_tab (id, value) values (1,1)",
    "insert into cursor_bind_err_tab (id, value) values (1,1)",
    "insert into cursor_bind_err_tab (id, value) values (1,1)",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

$sql = "select cursor(select * from cursor_bind_err_tab) into :cursor from dual";
$stmt = oci_parse($c, $sql);

$cursor = oci_new_cursor($c);
oci_bind_by_name($stmt, ":cursor", $cursor, -1, OCI_B_CURSOR);

oci_execute($stmt);

oci_execute($cursor);
var_dump(oci_fetch_assoc($cursor));

// Cleanup

$stmtarray = array(
    "drop table cursor_bind_err_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECTF--
Warning: oci_bind_by_name(): ORA-01036: %s in %s on line %d

Warning: oci_fetch_assoc(): ORA-24338: %s in %s on line %d
bool(false)
Done
