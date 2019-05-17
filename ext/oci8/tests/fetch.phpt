--TEST--
ocifetch() & ociresult()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(__DIR__."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table fetch_tab",
    "create table fetch_tab (id number, value number)",
    "insert into fetch_tab (id, value) values (1,1)",
    "insert into fetch_tab (id, value) values (1,1)",
    "insert into fetch_tab (id, value) values (1,1)",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

if (!($s = oci_parse($c, "select * from fetch_tab"))) {
	die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}

while(ocifetch($s)) {
		$row = ociresult($s, 1);
		$row1 = ociresult($s, 2);
		var_dump($row);
		var_dump($row1);
}

// Cleanup

$stmtarray = array(
    "drop table fetch_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";
?>
--EXPECT--
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
Done
