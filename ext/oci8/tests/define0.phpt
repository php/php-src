--TEST--
oci_define_by_name()
--EXTENSIONS--
oci8
--FILE--
<?php

require(__DIR__."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table define0_tab",
    "create table define0_tab (string varchar(10))",
    "insert into define0_tab (string) values ('some')",
);

oci8_test_sql_execute($c, $stmtarray);

// Run test

$stmt = oci_parse($c, "select string from define0_tab");

/* the define MUST be done BEFORE ociexecute! */

echo "Test 1\n";

$string = '';
oci_define_by_name($stmt, "STRING", $string, 20);
oci_execute($stmt);
while (oci_fetch($stmt)) {
    var_dump($string);
}

echo "Test 2\n";

$string = '';
$s2 = oci_parse($c, 'select string from define0_tab');
oci_define_by_name($s2, 'STRING', $string);
oci_execute($s2);
while (oci_fetch($s2)) {
    var_dump($string);
}

// Cleanup

$stmtarray = array(
    "drop table define0_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECTF--
Test 1
string(%d) "some"
Test 2
string(%d) "some"
