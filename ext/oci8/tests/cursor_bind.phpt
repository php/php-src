--TEST--
bind and fetch cursor from a statement
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__."/connect.inc");

// Initialization

$stmtarray = array(
    "drop table cursor_bind_tab",
    "create table cursor_bind_tab (id NUMBER, value VARCHAR(20))",
    "insert into cursor_bind_tab values (1, '1')",
    "insert into cursor_bind_tab values (1, '1')",
    "insert into cursor_bind_tab values (1, '1')"
);

oci8_test_sql_execute($c, $stmtarray);

$sql = "
DECLARE
TYPE curtype IS REF CURSOR;
cursor_var curtype;
BEGIN
    OPEN cursor_var FOR SELECT id, value FROM cursor_bind_tab;
    :curs := cursor_var;
END;
";

$stmt = oci_parse($c, $sql);

$cursor = oci_new_cursor($c);
oci_bind_by_name($stmt, ":curs", $cursor, -1, OCI_B_CURSOR);

oci_execute($stmt);

oci_execute($cursor);
var_dump(oci_fetch_row($cursor));
var_dump(oci_fetch_row($cursor));
var_dump(oci_fetch_row($cursor));
var_dump(oci_fetch_row($cursor));

// Clean up

$stmtarray = array(
    "drop table cursor_bind_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
bool(false)
===DONE===
