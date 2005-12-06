--TEST--
fetch after failed oci_execute()
---SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

$sql = "select 2 from nonex_dual";
$stmt = oci_parse($c, $sql);

var_dump(oci_execute($stmt));
var_dump(oci_fetch_array($stmt));

echo "Done\n";
?>
--EXPECTF--	
Done
