--TEST--
oci_define_by_name() for statement re-execution
--EXTENSIONS--
oci8
--FILE--
<?php

require(__DIR__."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table define5_tab",
    "create table define5_tab (id number, string varchar(10))",
    "insert into define5_tab (id, string) values (1, 'some')",
    "insert into define5_tab (id, string) values (2, 'thing')",
);

oci8_test_sql_execute($c, $stmtarray);

// Run test

echo "Test 1 - must do define before execute\n";
$stmt = oci_parse($c, "select string from define5_tab where id = 1");
oci_execute($stmt);
var_dump(oci_define_by_name($stmt, "STRING", $string));
while (oci_fetch($stmt)) {
    var_dump($string);  // gives NULL
    var_dump(oci_result($stmt, 'STRING'));
}

echo "Test 2 - normal define order\n";
$stmt = oci_parse($c, "select string from define5_tab where id = 1");
var_dump(oci_define_by_name($stmt, "STRING", $string));
oci_execute($stmt);

while (oci_fetch($stmt)) {
    var_dump($string);
}

echo "Test 3 - no new define done\n";
$stmt = oci_parse($c, "select string from define5_tab where id = 2");
oci_execute($stmt);
while (oci_fetch($stmt)) {
    var_dump($string); // not updated with new value
    var_dump(oci_result($stmt, 'STRING'));
}

// Cleanup

$stmtarray = array(
    "drop table define5_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECT--
Test 1 - must do define before execute
bool(true)
NULL
string(4) "some"
Test 2 - normal define order
bool(true)
string(4) "some"
Test 3 - no new define done
string(4) "some"
string(5) "thing"
Done
