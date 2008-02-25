--TEST--
Bug #44206 (Test if selecting ref cursors leads to ORA-1000 maximum open cursors reached)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Run Test

for ($x = 0; $x < 400; $x++) {
	$stmt = "select cursor (select $x from dual) a,
		 			cursor (select $x from dual) b 
		 	 from dual";
	$s = oci_parse($c, $stmt);
	$r = oci_execute($s);
	if (!$r) {
		echo "Exiting $x\n";
		exit;
	}
	$result = oci_fetch_array($s, OCI_ASSOC);
	oci_execute($result['A']);
	oci_execute($result['B']);
	oci_fetch_array($result['A'], OCI_ASSOC);
	oci_fetch_array($result['B'], OCI_ASSOC);
	oci_free_statement($result['A']);
	oci_free_statement($result['B']);
	oci_free_statement($s);
}

echo "Completed $x\n";

oci_close($c);

echo "Done\n";

?>
--EXPECT--
Completed 400
Done
