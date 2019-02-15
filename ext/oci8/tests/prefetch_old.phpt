--TEST--
ocisetprefetch()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

$stmtarray = array(
    "drop table prefetch_old_tab",
    "create table prefetch_old_tab (id number, value number)",
    "insert into prefetch_old_tab (id, value) values (1,1)",
    "insert into prefetch_old_tab (id, value) values (1,1)",
    "insert into prefetch_old_tab (id, value) values (1,1)",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test

if (!ocicommit($c)) {
	die("ocicommit() failed!\n");
}

$select_sql = "select * from prefetch_old_tab";

if (!($s = ociparse($c, $select_sql))) {
	die("ociparse(select) failed!\n");
}

var_dump(ocisetprefetch($s, 10));

if (!ociexecute($s)) {
	die("ociexecute(select) failed!\n");
}

var_dump(ocifetch($s));
var_dump(ocirowcount($s));


// Cleanup

$stmtarray = array(
    "drop table prefetch_old_tab"
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";
?>
--EXPECT--
bool(true)
bool(true)
int(1)
Done
