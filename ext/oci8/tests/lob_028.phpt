--TEST--
Test descriptor types for oci_new_descriptor()
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

// Successful statements

$d = oci_new_descriptor($c, OCI_D_FILE);
var_dump($d);

$d = oci_new_descriptor($c, OCI_DTYPE_FILE);
var_dump($d);

$d = oci_new_descriptor($c, OCI_D_LOB);
var_dump($d);

$d = oci_new_descriptor($c, OCI_DTYPE_LOB);
var_dump($d);

$d = oci_new_descriptor($c, OCI_D_ROWID);
var_dump($d);

$d = oci_new_descriptor($c, OCI_DTYPE_ROWID);
var_dump($d);

// Unsuccessful statements

$d = oci_new_descriptor($c, OCI_B_CLOB);
var_dump($d);

$d = oci_new_descriptor($c, OCI_B_CLOB);
var_dump($d);

$d = oci_new_descriptor($c, OCI_DEFAULT);
var_dump($d);

$d = oci_new_descriptor($c, 1);
var_dump($d);

echo "Done\n";

?>
--EXPECTF--
object(OCILob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
object(OCILob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
object(OCILob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
object(OCILob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
object(OCILob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
object(OCILob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}

Warning: oci_new_descriptor(): Unknown descriptor type %d in %s on line %d
NULL

Warning: oci_new_descriptor(): Unknown descriptor type %d in %s on line %d
NULL

Warning: oci_new_descriptor(): Unknown descriptor type %d in %s on line %d
NULL

Warning: oci_new_descriptor(): Unknown descriptor type %d in %s on line %d
NULL
Done
