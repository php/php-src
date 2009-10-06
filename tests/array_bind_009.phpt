--TEST--
oci_bind_array_by_name() and invalid values 9
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

var_dump(oci_bind_array_by_name($c, ":c1", $array, 5, 5, SQLT_CHR));

echo "Done\n";
?>
--EXPECTF--	
Warning: oci_bind_array_by_name(): supplied resource is not a valid oci8 statement resource in %s on line %d
bool(false)
Done
