--TEST--
fetching the same lob several times
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table lob_018_tab",
	"create table lob_018_tab (mykey number, lob_1 clob)",
);

oci8_test_sql_execute($c, $stmtarray);

echo "Test 1\n";

$init = "insert into lob_018_tab (mykey, lob_1) values(1, empty_clob()) returning lob_1 into :mylob";
$statement = oci_parse($c, $init);
$clob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($statement, ":mylob", $clob, -1, OCI_B_CLOB);
oci_execute($statement, OCI_DEFAULT);
$clob->save("data");

oci_commit($c);

$init = "insert into lob_018_tab (mykey, lob_1) values(2, empty_clob()) returning lob_1 into :mylob";
$statement = oci_parse($c, $init);
$clob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($statement, ":mylob", $clob, -1, OCI_B_CLOB);
oci_execute($statement, OCI_DEFAULT);
$clob->save("long data");

oci_commit($c);


$query = 'select * from lob_018_tab order by mykey asc';
$statement = oci_parse ($c, $query);
oci_execute($statement, OCI_DEFAULT);

while ($row = oci_fetch_array($statement, OCI_ASSOC)) {
	$result = $row['LOB_1']->load();
	var_dump($result);
}

echo "Test 2\n";

$query = 'select * from lob_018_tab order by mykey desc';
$statement = oci_parse ($c, $query);
oci_execute($statement, OCI_DEFAULT);

while ($row = oci_fetch_array($statement, OCI_ASSOC)) {
	$result = $row['LOB_1']->load();
	var_dump($result);
}

echo "Test 3 - bind with SQLT_CLOB (an alias for OCI_B_CLOB)\n";

$init = "insert into lob_018_tab (mykey, lob_1) values(3, empty_clob()) returning lob_1 into :mylob";
$statement = oci_parse($c, $init);
$clob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($statement, ":mylob", $clob, -1, SQLT_CLOB);
oci_execute($statement, OCI_DEFAULT);
$clob->save("more stuff");

oci_commit($c);

$query = 'select * from lob_018_tab where mykey = 3';
$statement = oci_parse ($c, $query);
oci_execute($statement, OCI_DEFAULT);

while ($row = oci_fetch_array($statement, OCI_ASSOC)) {
	$result = $row['LOB_1']->load();
	var_dump($result);
}

// Cleanup

$stmtarray = array(
    "drop table lob_018_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Test 1
string(4) "data"
string(9) "long data"
Test 2
string(9) "long data"
string(4) "data"
Test 3 - bind with SQLT_CLOB (an alias for OCI_B_CLOB)
string(10) "more stuff"
===DONE===
