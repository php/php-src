--TEST--
oci_bind_array_by_name(), SQLT_CHR, default max_length and empty array
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

$statement = oci_parse($c, 'SELECT user FROM all_objects');

$array = array();

oci_bind_array_by_name($statement, ":c1", $array, 5, -10, SQLT_CHR);

oci_execute($statement);

var_dump($array);

echo "Done\n";
?>
--EXPECTF--
Warning: oci_bind_array_by_name(): Invalid max length value (-10) in %s on line %d
array(0) {
}
Done
