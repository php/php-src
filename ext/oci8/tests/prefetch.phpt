--TEST--
oci_set_prefetch()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table prefetch_tab",
    "create table prefetch_tab (id number, value number)",
    "insert into prefetch_tab (id, value) values (1,1)",
    "insert into prefetch_tab (id, value) values (1,1)",
    "insert into prefetch_tab (id, value) values (1,1)",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

$select_sql = "select * from prefetch_tab";

if (!($s = oci_parse($c, $select_sql))) {
	die("oci_parse(select) failed!\n");
}

var_dump(oci_set_prefetch($s, 10));

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}

var_dump(oci_fetch($s));
var_dump(oci_num_rows($s));

// Cleanup

$stmtarray = array(
    "drop table prefetch_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";
?>
--EXPECT--
bool(true)
bool(true)
int(1)
Done
