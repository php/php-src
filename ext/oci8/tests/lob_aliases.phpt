--TEST--
LOB method aliases
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

// Function existence
echo "Test 1\n";
var_dump(function_exists('oci_lob_load'));
var_dump(function_exists('oci_lob_tell'));
var_dump(function_exists('oci_lob_truncate'));
var_dump(function_exists('oci_lob_erase'));
var_dump(function_exists('oci_lob_flush'));
var_dump(function_exists('ocisetbufferinglob'));
var_dump(function_exists('ocigetbufferinglob'));
var_dump(function_exists('oci_lob_rewind'));
var_dump(function_exists('oci_lob_read'));
var_dump(function_exists('oci_lob_eof'));
var_dump(function_exists('oci_lob_seek'));
var_dump(function_exists('oci_lob_write'));
var_dump(function_exists('oci_lob_append'));
var_dump(function_exists('oci_lob_size'));
var_dump(function_exists('oci_lob_export'));
var_dump(function_exists('oci_lob_export'));
var_dump(function_exists('oci_lob_import'));
var_dump(function_exists('oci_lob_save'));
var_dump(function_exists('oci_lob_import'));
var_dump(function_exists('oci_free_descriptor'));

echo "Done\n";

?>
--EXPECT--
Test 1
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Done
