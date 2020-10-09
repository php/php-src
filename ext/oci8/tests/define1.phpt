--TEST--
oci_define_by_name()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table define1_tab",
    "create table define1_tab (string varchar(10))",
    "insert into define1_tab (string) values ('some')",
);

oci8_test_sql_execute($c, $stmtarray);

// Run test

$stmt = oci_parse($c, "select string from define1_tab");

/* the define MUST be done BEFORE ociexecute! */

$string = '';
var_dump(oci_define_by_name($stmt, "STRING", $string, 20));
var_dump(oci_define_by_name($stmt, "STRING", $string, 20));
try {
    var_dump(oci_define_by_name($stmt, "", $string, 20));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

oci_execute($stmt);

while (oci_fetch($stmt)) {
    var_dump($string);
}

// Cleanup

$stmtarray = array(
    "drop table define1_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECTF--
bool(true)
bool(false)
oci_define_by_name(): Argument #2 ($column) cannot be empty
string(4) "some"
Done
