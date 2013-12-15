--TEST--
ocidefinebyname()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table define_old_tab",
    "create table define_old_tab (string varchar(10))",
    "insert into define_old_tab (string) values ('some')",
);

oci8_test_sql_execute($c, $stmtarray);

// Run test

$stmt = ociparse($c, "select string from define_old_tab");

/* the define MUST be done BEFORE ociexecute! */

$string = '';
ocidefinebyname($stmt, "STRING", $string, 20);

ociexecute($stmt);

while (ocifetch($stmt)) {
	var_dump($string);
}

// Cleanup

$stmtarray = array(
    "drop table define_old_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECTF--
string(4) "some"
Done
