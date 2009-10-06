--TEST--
Bug #44113 (New collection creation can fail with OCI-22303)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"create or replace type bug44113_list_t as table of number"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	@oci_execute($s);
}

// Run Test
// The test can take some time to complete and can exceed PHP's test
// timout limit on slow networks.

for ($x = 0; $x < 70000; $x++) {
	if (!($var = oci_new_collection($c, 'BUG44113_LIST_T'))) {
		print "Failed new collection creation on $x\n";
		break;
	}
}

print "Completed $x\n";

// Cleanup

$stmtarray = array(
	"drop type bug44113_list_t"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

oci_close($c);

echo "Done\n";

?>
--EXPECT--
Completed 70000
Done
