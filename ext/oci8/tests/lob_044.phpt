--TEST--
oci_lob_truncate() with default parameter value
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?> 
--FILE--
<?php
	
require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table lob_044_tab",
	"create table lob_044_tab (blob BLOB)",
);

oci8_test_sql_execute($c, $stmtarray);


// Run Test

echo "Test 1 - truncate on insert\n";

$s = oci_parse($c, "INSERT INTO lob_044_tab (blob) VALUES (empty_blob()) RETURNING blob INTO :v_blob ");
$blob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s,":v_blob", $blob, -1, OCI_B_BLOB);
oci_execute($s, OCI_DEFAULT);

var_dump($blob->write("this is a biiiig faaat test string. why are you reading it, I wonder? =)"));
var_dump($blob->seek(0));
var_dump($blob->read(10000));
var_dump($blob->truncate());
var_dump($blob->seek(0));
var_dump($blob->read(10000));

oci_commit($c);


// Read it back

echo "\nTest 2 - read it back\n";

$s = oci_parse($c, "SELECT blob FROM lob_044_tab FOR UPDATE");
oci_execute($s, OCI_DEFAULT);
$row = oci_fetch_array($s);
var_dump($row[0]->read(10000));

// Clean up

$stmtarray = array(
	"drop table lob_044_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1 - truncate on insert
int(72)
bool(true)
string(72) "this is a biiiig faaat test string. why are you reading it, I wonder? =)"
bool(true)
bool(true)
string(0) ""

Test 2 - read it back
string(0) ""
===DONE===
