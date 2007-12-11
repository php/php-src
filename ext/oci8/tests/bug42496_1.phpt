--TEST--
Bug #42496 (LOB fetch leaks cursors, eventually failing with ORA-1000 maximum open cursors reached)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

// Initialization

$stmtarray = array(
	"DROP table bug42496_tab",
	"CREATE table bug42496_tab(c1 CLOB, c2 CLOB)",
	"INSERT INTO bug42496_tab VALUES('test1', 'test1')",
	"INSERT INTO bug42496_tab VALUES('test2', 'test2')",
	"INSERT INTO bug42496_tab VALUES('test3', 'test3')"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	@oci_execute($s);
}

// Run Test

echo "Test 1\n";

for ($i = 0; $i < 15000; $i++) {
	$s = oci_parse($c, "SELECT * from bug42496_tab");
	oci_define_by_name($s, "C1", $col1);
	oci_define_by_name($s, "C2", $col2);
	if (oci_execute($s)) {
		$arr = array();
		while ($arr = oci_fetch_assoc($s)) {
			$arr['C1']->free();
			$arr['C2']->free();
		}
	}
	oci_free_statement($s);
}

echo "Done\n";

// Cleanup

$stmtarray = array(
	"DROP table bug42496_tab"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	@oci_execute($s);
}

oci_close($c);

?>
--EXPECTF--
Test 1
Done
