--TEST--
Bug #26133 (ocifreedesc() segfault)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialize

$stmtarray = array(
    "drop table bug26133_tab",
    "create table bug26133_tab (id number, value number)",
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

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

// Run Test

$ora_sql = "INSERT INTO bug26133_tab (id, value) VALUES ('1','1') RETURNING ROWID INTO :v_rowid ";

$statement = OCIParse($c,$ora_sql);
$rowid = OCINewDescriptor($c,OCI_D_ROWID);
OCIBindByName($statement,":v_rowid", $rowid,-1,OCI_B_ROWID);
if (OCIExecute($statement)) {
    OCICommit($c);
}
OCIFreeStatement($statement);
$rowid->free();

// Cleanup

$stmtarray = array(
    "drop table bug26133_tab"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}
                         
echo "Done\n";
?>
--EXPECT--
Done
