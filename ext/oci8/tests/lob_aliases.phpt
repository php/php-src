--TEST--
LOB method aliases
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

// Function existence
echo "Test 1\n";
var_dump(oci_lob_load());
var_dump(oci_lob_tell());
var_dump(oci_lob_truncate());
var_dump(oci_lob_erase());
var_dump(oci_lob_flush());
var_dump(ocisetbufferinglob());
var_dump(ocigetbufferinglob());
var_dump(oci_lob_rewind());
var_dump(oci_lob_read());
var_dump(oci_lob_eof());
var_dump(oci_lob_seek());
var_dump(oci_lob_write());
var_dump(oci_lob_append());
var_dump(oci_lob_size());
var_dump(oci_lob_export());
var_dump(oci_lob_export());
var_dump(oci_lob_import());
// No PHP_FE for oci_lob_write_temporary() or oci_lob_close()
//var_dump(oci_lob_write_temporary());
//var_dump(oci_lob_close());
var_dump(oci_lob_save());
var_dump(oci_lob_import());
var_dump(oci_free_descriptor());

echo "Done\n";

?>
--EXPECTF--
Test 1

Warning: oci_lob_load() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: oci_lob_tell() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: oci_lob_truncate() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: oci_lob_erase() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: oci_lob_flush() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: ocisetbufferinglob() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: ocigetbufferinglob() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: oci_lob_rewind() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: oci_lob_read() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: oci_lob_eof() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: oci_lob_seek() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: oci_lob_write() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: oci_lob_append() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: oci_lob_size() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: oci_lob_export() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: oci_lob_export() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: oci_lob_import() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: oci_lob_save() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: oci_lob_import() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: oci_free_descriptor() expects exactly 1 parameter, 0 given in %s on line %d
NULL
Done
