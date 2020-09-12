--TEST--
oci_lob_copy() - 2
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__.'/connect.inc';
require __DIR__.'/create_table.inc';

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

echo "Writing blob\n";
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

//--------------------------------------------------

echo "\noci_lob_copy invalid args\n";

var_dump(oci_lob_copy($dummy, $row1[0]));
var_dump(oci_lob_copy($row2[0], $dummy));
var_dump(oci_lob_copy($row2[0], $row1[0], 0));
var_dump(oci_lob_copy($row2[0], $row1[0], 100000));

try {
    var_dump(oci_lob_copy($row2[0], $row1[0], -1));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

//--------------------------------------------------

echo "\noci_lob_size tests\n";

var_dump(oci_lob_size($row2[0]));
unset($dummy->descriptor);
var_dump(oci_lob_size($dummy));

oci_rollback($c);

//--------------------------------------------------

echo "\nQuery test\n";

$select_sql = "SELECT blob FROM ".$schema.$table_name." WHERE id = 2 FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

var_dump($row2 = oci_fetch_array($s, OCI_RETURN_LOBS));

require __DIR__.'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
Writing blob
int(32)

oci_lob_copy invalid args

Warning: oci_lob_copy(): OCI_INVALID_HANDLE in %s on line %d
bool(false)

Warning: oci_lob_copy(): OCI_INVALID_HANDLE in %s on line %d
bool(false)
bool(false)
bool(true)
oci_lob_copy(): Argument #3 ($length) must be greater than or equal to 0

oci_lob_size tests
int(0)

Warning: oci_lob_size(): Unable to find descriptor property in %s on line %d
bool(false)

Query test
array(2) {
  [0]=>
  string(0) ""
  ["BLOB"]=>
  string(0) ""
}
Done
