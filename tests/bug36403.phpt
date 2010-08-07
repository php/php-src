--TEST--
Bug #36403 (oci_execute no longer supports OCI_DESCRIBE_ONLY)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table bug36403_tab",
	"create table bug36403_tab (c1 number, col2 number, column3 number, col4 number)"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	$r = @oci_execute($s);
	if (!$r) {
		$m = oci_error($s);
		if (!in_array($m['code'], array(   // ignore expected errors
				942 // table or view does not exist
			,  2289 // sequence does not exist
			,  4080 // trigger does not exist
			, 38802 // edition does not exist
				))) {
			echo $stmt . PHP_EOL . $m['message'] . PHP_EOL;
		}
	}
}

// Run Test

echo "Test 1\n";

$s = oci_parse($c, "select * from bug36403_tab");
oci_execute($s, OCI_DESCRIBE_ONLY);
for ($i = oci_num_fields($s); $i > 0; $i--) {
	echo oci_field_name($s, $i) . "\n";
}

echo "Test 2\n";

// Should generate an error: ORA-24338: statement handle not executed
// since the statement handle was only described and not executed
$row = oci_fetch_array($s);

// Clean up

//require(dirname(__FILE__).'/drop_table.inc');

$stmtarray = array(
	"drop table bug36403_tab"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

oci_close($c);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1
COL4
COLUMN3
COL2
C1
Test 2

Warning: oci_fetch_array(): ORA-24338: %sbug36403.php on line %d
===DONE===
