--TEST--
Bug #32325 (Cannot retrieve collection using OCI8)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');
		
// Initialize

$stmtarray = array(
    "create or replace type bug32325_t as table of number"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	$r = @oci_execute($s);
	if (!$r) {
		$m = oci_error($s);
		if (!in_array($m['code'], array(   // ignore expected errors
                    942 // table or view does not exist
				))) {
			echo $stmt . PHP_EOL . $m['message'] . PHP_EOL;
		}
	}
}

// Run test
	
$collection = oci_new_collection($c, "BUG32325_T");

$sql = "begin
		select bug32325_t(1,2,3,4) into :list from dual;
		end;";

$stmt = oci_parse($c, $sql);

oci_bind_by_name($stmt, ":list",  $collection, -1, OCI_B_NTY);
oci_execute($stmt);

var_dump($collection->size());
var_dump($collection->getelem(1));
var_dump($collection->getelem(2));

// Cleanup

$stmtarray = array(
    "drop type bug32325_t"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

echo "Done\n";
?>
--EXPECT--	
int(4)
float(2)
float(3)
Done
