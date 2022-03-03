--TEST--
oci_define_by_name() on partial number of columns
--EXTENSIONS--
oci8
--FILE--
<?php

require(__DIR__."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table define4_tab",
    "create table define4_tab (value number, string varchar(10))",
    "insert into define4_tab (value, string) values (1234, 'some')",
);

oci8_test_sql_execute($c, $stmtarray);

// Run test

$stmt = oci_parse($c, "select value, string from define4_tab");

echo "Test 1\n";
// Only one of the two columns is defined
var_dump(oci_define_by_name($stmt, "STRING", $string));

oci_execute($stmt);

echo "Test 2\n";

while (oci_fetch($stmt)) {
    var_dump(oci_result($stmt, 'VALUE'));
    var_dump($string);
    var_dump(oci_result($stmt, 'STRING'));
    var_dump($string);
    var_dump(oci_result($stmt, 'VALUE'));
    var_dump(oci_result($stmt, 'STRING'));
}

echo "Test 3\n";
var_dump(oci_free_statement($stmt));
var_dump($string);

try {
    var_dump(oci_result($stmt, 'STRING'));
} catch(\TypeError $exception) {
    var_dump($exception->getMessage());
}
// Cleanup

$stmtarray = array(
    "drop table define4_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECTF--
Test 1
bool(true)
Test 2
string(4) "1234"
string(4) "some"
string(4) "some"
string(4) "some"
string(4) "1234"
string(4) "some"
Test 3
bool(true)
string(4) "some"
string(%d) "oci_result(): supplied resource is not a valid oci8 statement resource"
Done
