--TEST--
oci_bind_array_by_name(), SQLT_CHR, default max_length and empty array
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

$statement = oci_parse($c, 'SELECT user FROM v$session');

$array = array();

try {
    var_dump(oci_bind_array_by_name($statement, ":c1", $array, 5, -10, SQLT_CHR, -10));
} catch (ArgumentCountError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(oci_bind_array_by_name($statement, ":c1", $array, 5, -10));
var_dump(oci_bind_array_by_name($statement, ":c1", $array, 5, -1));
var_dump(oci_bind_array_by_name($statement, ":c1", $array, 5, 0));

@oci_execute($statement);

var_dump($array);

echo "Done\n";
?>
--EXPECTF--
oci_bind_array_by_name() expects at most 6 arguments, 7 given

Warning: oci_bind_array_by_name(): You must provide max length value for empty arrays in %s on line %d
bool(false)

Warning: oci_bind_array_by_name(): You must provide max length value for empty arrays in %s on line %d
bool(false)

Warning: oci_bind_array_by_name(): You must provide max length value for empty arrays in %s on line %d
bool(false)
array(0) {
}
Done
