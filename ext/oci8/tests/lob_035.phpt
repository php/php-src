--TEST--
oci_lob_copy() - 2
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';
require dirname(__FILE__).'/create_table.inc';

$ora_sql = "INSERT INTO
                       ".$schema.$table_name." (id, blob)
                      VALUES (1, empty_blob())
                      RETURNING
                               blob
                      INTO :v_blob ";

$statement = oci_parse($c,$ora_sql);
$blob = oci_new_descriptor($c,OCI_D_LOB);
oci_bind_by_name($statement,":v_blob", $blob,-1,OCI_B_BLOB);
oci_execute($statement, OCI_DEFAULT);

var_dump($blob->write("some string here. string, I said"));
oci_commit($c);

$ora_sql = "INSERT INTO
                       ".$schema.$table_name." (id, blob)
                      VALUES (2, empty_blob())
                      RETURNING
                               blob
                      INTO :v_blob ";

$statement = oci_parse($c,$ora_sql);
$blob = oci_new_descriptor($c,OCI_D_LOB);
oci_bind_by_name($statement,":v_blob", $blob,-1,OCI_B_BLOB);
oci_execute($statement, OCI_DEFAULT);

oci_commit($c);

$select_sql = "SELECT blob FROM ".$schema.$table_name." WHERE id = 1";
$s = oci_parse($c, $select_sql);
oci_execute($s);

$row1 = oci_fetch_array($s);

$select_sql = "SELECT blob FROM ".$schema.$table_name." WHERE id = 2 FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

$row2 = oci_fetch_array($s);

$dummy = oci_new_descriptor($c, OCI_D_LOB);

var_dump(oci_lob_copy($dummy, $row1[0]));
var_dump(oci_lob_copy($row2[0], $dummy));

var_dump(oci_lob_copy($row2[0], $row1[0], 0));
var_dump(oci_lob_copy($row2[0], $row1[0], -1));
var_dump(oci_lob_copy($row2[0], $row1[0], 100000));

var_dump(oci_lob_size());
var_dump(oci_lob_size($row2[0]));
unset($dummy->descriptor);
var_dump(oci_lob_size($dummy));

oci_rollback($c);
oci_rollback($c);
oci_commit($c);
oci_commit($c);

$select_sql = "SELECT blob FROM ".$schema.$table_name." WHERE id = 2 FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

var_dump($row2 = oci_fetch_array($s, OCI_RETURN_LOBS));

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
int(32)

Warning: oci_lob_copy(): OCI_INVALID_HANDLE in %s on line %d
bool(false)

Warning: oci_lob_copy(): OCI_INVALID_HANDLE in %s on line %d
bool(false)
bool(false)

Warning: oci_lob_copy(): Length parameter must be greater than 0 in %s on line %d
bool(false)
bool(true)

Warning: oci_lob_size() expects exactly 1 parameter, 0 given in %s on line %d
NULL
int(0)

Warning: oci_lob_size(): Unable to find descriptor property in %s on line %d
bool(false)
array(2) {
  [0]=>
  string(0) ""
  ["BLOB"]=>
  string(0) ""
}
Done
