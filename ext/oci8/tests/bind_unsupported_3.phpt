--TEST--
Bind with various bind types not supported by TimesTen
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => false, 'timesten' => true);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

$types = array(
    "SQLT_CLOB" => SQLT_CLOB,
    "SQLT_BLOB" => SQLT_BLOB,
    "OCI_B_CLOB" => OCI_B_CLOB,
    "OCI_B_BLOB" => OCI_B_BLOB,
);

foreach ($types as $t => $v) {

    echo "Test - $t\n";

    $s = oci_parse($c, "select * from dual where dummy = :c1");
    $c1 = "Doug";
    oci_bind_by_name($s, ":c1", $c1, -1, $v);
}

?>
--EXPECTF--
Test - SQLT_CLOB

Warning: oci_bind_by_name(): Unable to find descriptor property in %sbind_unsupported_3.php on line %d
Test - SQLT_BLOB

Warning: oci_bind_by_name(): Unable to find descriptor property in %sbind_unsupported_3.php on line %d
Test - OCI_B_CLOB

Warning: oci_bind_by_name(): Unable to find descriptor property in %sbind_unsupported_3.php on line %d
Test - OCI_B_BLOB

Warning: oci_bind_by_name(): Unable to find descriptor property in %sbind_unsupported_3.php on line %d
