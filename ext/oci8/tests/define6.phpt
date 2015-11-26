--TEST--
oci_define_by_name tests with REF CURSORs
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?> 
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
    "drop table define6_tab",
    "create table define6_tab (id number)",
    "insert into define6_tab values (1)"
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

$sql = 
"DECLARE
  TYPE curtype IS REF CURSOR;
  cursor_var curtype;
BEGIN
  OPEN cursor_var FOR SELECT id FROM define6_tab;
  :curs := cursor_var;
END;";

echo "Test 1 - define last\n";

$s1 = oci_parse($c, $sql);
$cursor1 = oci_new_cursor($c);
oci_bind_by_name($s1, ":curs", $cursor1, -1, OCI_B_CURSOR);
oci_execute($s1);
oci_execute($cursor1);
oci_define_by_name($cursor1, 'ID', $id1);
while (oci_fetch_row($cursor1)) {
    var_dump($id1);
}


echo "Test 2 - define last with preset var\n";

$s2 = oci_parse($c, $sql);
$cursor2 = oci_new_cursor($c);
oci_bind_by_name($s2, ":curs", $cursor2, -1, OCI_B_CURSOR);
oci_execute($s2);
oci_execute($cursor2);
$id2 = '';
oci_define_by_name($cursor2, 'ID', $id2);
while (oci_fetch_row($cursor2)) {
    var_dump($id2);
}


echo "Test 3 - define before cursor execute\n";

$s3 = oci_parse($c, $sql);
$cursor3 = oci_new_cursor($c);
oci_bind_by_name($s3, ":curs", $cursor3, -1, OCI_B_CURSOR);
oci_execute($s3);
oci_define_by_name($cursor3, 'ID', $id3);
oci_execute($cursor3);
while (oci_fetch_row($cursor3)) {
    var_dump($id3);
}


echo "Test 4 - define before top level execute\n";

$s4 = oci_parse($c, $sql);
$cursor4 = oci_new_cursor($c);
oci_bind_by_name($s4, ":curs", $cursor4, -1, OCI_B_CURSOR);
oci_define_by_name($cursor4, 'ID', $id4);
oci_execute($s4);
oci_execute($cursor4);
while (oci_fetch_row($cursor4)) {
    var_dump($id4);
}


echo "Test 5 - define before bind\n";

$s5 = oci_parse($c, $sql);
$cursor5 = oci_new_cursor($c);
oci_define_by_name($cursor5, 'ID', $id5);
oci_bind_by_name($s5, ":curs", $cursor5, -1, OCI_B_CURSOR);
oci_execute($s5);
oci_execute($cursor5);
while (oci_fetch_row($cursor5)) {
    var_dump($id5);
}


echo "Test 6 - fetch on wrong handle\n";

$s6 = oci_parse($c, $sql);
$cursor6 = oci_new_cursor($c);
oci_define_by_name($cursor6, 'ID', $id6);
oci_bind_by_name($s6, ":curs", $cursor6, -1, OCI_B_CURSOR);
oci_execute($s6);
oci_execute($cursor6);
while (oci_fetch_row($s6)) {
    var_dump($id6);
}


// Clean up

$stmtarray = array(
    "drop table define6_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1 - define last
NULL
Test 2 - define last with preset var
string(0) ""
Test 3 - define before cursor execute
string(1) "1"
Test 4 - define before top level execute
string(1) "1"
Test 5 - define before bind
string(1) "1"
Test 6 - fetch on wrong handle

Warning: oci_fetch_row(): ORA-24374: %s in %sdefine6.php on line %d
===DONE===
