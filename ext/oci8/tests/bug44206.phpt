--TEST--
Bug #44206 (Test if selecting ref cursors leads to ORA-1000 maximum open cursors reached)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

// Run Test

for ($x = 0; $x < 400; $x++)
{
	$stmt = "select cursor (select $x from dual) a,
		 cursor (select $x from dual) b
		 from dual";
	$s = oci_parse($c, $stmt);
	$r = oci_execute($s);
        if (!$r) {
                echo "Exiting $x\n";
                exit;
        }
	$mode = OCI_ASSOC | OCI_RETURN_NULLS;
	$result = oci_fetch_array($s, $mode);
	oci_execute($result['A']);
	oci_execute($result['B']);
	oci_fetch_array($result['A'], $mode);
	oci_fetch_array($result['B'], $mode);
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
