--TEST--
oci8.default_prefetch ini option
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--INI--
oci8.default_prefetch=100
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table default_prefetch1_tab",
    "create table default_prefetch1_tab (id number, value number)",
    "insert into default_prefetch1_tab (id, value) values (1,1)",
    "insert into default_prefetch1_tab (id, value) values (1,1)",
    "insert into default_prefetch1_tab (id, value) values (1,1)",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

$select_sql = "select * from default_prefetch1_tab";

if (!($s = oci_parse($c, $select_sql))) {
	die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}

var_dump(oci_fetch($s));
var_dump(oci_num_rows($s));


// Cleanup

$stmtarray = array(
    "drop table default_prefetch1_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";
?>
--EXPECT--
bool(true)
int(1)
Done
