--TEST--
oci_define_by_name()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table define_tab",
    "create table define_tab (string varchar(10))",
    "insert into define_tab (string) values ('some')",
);

oci8_test_sql_execute($c, $stmtarray);

// Run test

$stmt = oci_parse($c, "select string from define_tab");

/* the define MUST be done BEFORE ociexecute! */

$string = '';
oci_define_by_name($stmt, "STRING", $string, 20);

oci_execute($stmt);

while (oci_fetch($stmt)) {
	var_dump($string);
}

// Cleanup

$stmtarray = array(
    "drop table define_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECTF--
%unicode|string%(%d) "some"
Done
