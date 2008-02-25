--TEST--
Bug #44008 (Incorrect usage of OCI-Lob->close doesn't crash PHP)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

// Initialization

$stmtarray = array(
		"create or replace procedure bug44008_proc (p in out clob)
		as begin p := 'A';
		end;"
);

foreach ($stmtarray as $stmt) {
        $s = oci_parse($c, $stmt);
        @oci_execute($s);
}

// Run Test

$s = oci_parse($c, 'begin bug44008_proc(:data); end;');
$textLob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ":data", $textLob, -1, OCI_B_CLOB);
oci_execute($s, OCI_DEFAULT);
$r = $textLob->load();
echo "$r\n";

// Incorrectly closing the lob doesn't cause a crash.
// OCI-LOB->close() is documented for use only with OCI-Lob->writeTemporary()
$textLob->close();

// Cleanup

$stmtarray = array(
        "drop procedure bug44008_proc"
);

foreach ($stmtarray as $stmt) {
        $s = oci_parse($c, $stmt);
        oci_execute($s);
}

oci_close($c);

echo "Done\n";

?>
--EXPECT--
A
Done
