--TEST--
oci_bind_array_by_name(), SQLT_CHR, default max_length and empty array
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

$statement = oci_parse($c, 'SELECT user FROM v$session');

$array = array();

var_dump(oci_bind_array_by_name($statement, ":c1", $array, 5, -10, SQLT_CHR, -10));
var_dump(oci_bind_array_by_name($statement, ":c1", $array, 5, -10));
var_dump(oci_bind_array_by_name($statement, ":c1", $array, 5, -1));
var_dump(oci_bind_array_by_name($statement, ":c1", $array, 5, 0));

@oci_execute($statement);

var_dump($array);

echo "Done\n";
?>
--EXPECTF--	
Warning: oci_bind_array_by_name() expects at most 6 parameters, 7 given in %s on line %d
NULL

Warning: oci_bind_array_by_name(): You must provide max length value for empty arrays in %s on line %d
bool(false)

Warning: oci_bind_array_by_name(): You must provide max length value for empty arrays in %s on line %d
bool(false)

Warning: oci_bind_array_by_name(): You must provide max length value for empty arrays in %s on line %d
bool(false)
array(0) {
}
Done
