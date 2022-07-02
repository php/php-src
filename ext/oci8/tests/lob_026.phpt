--TEST--
oci_lob_seek()/rewind()/append()
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
require __DIR__.'/create_table.inc';

$ora_sql = "INSERT INTO
                       ".$schema.$table_name." (blob)
                      VALUES (empty_blob())
                      RETURNING
                               blob
                      INTO :v_blob ";

$statement = oci_parse($c,$ora_sql);
$blob = oci_new_descriptor($c,OCI_D_LOB);
oci_bind_by_name($statement,":v_blob", $blob,-1,OCI_B_BLOB);
oci_execute($statement, OCI_DEFAULT);

var_dump($blob);

var_dump(oci_lob_write($blob, "test"));
var_dump(oci_lob_rewind($blob));
var_dump(oci_lob_write($blob, "str"));
var_dump(oci_lob_seek($blob, 10, OCI_SEEK_SET));

oci_commit($c);

$select_sql = "SELECT blob FROM ".$schema.$table_name." FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

var_dump($row = oci_fetch_array($s));
var_dump(oci_lob_append($row[0], $blob));
var_dump(oci_lob_read($row[0], 10000));

oci_commit($c);

$select_sql = "SELECT blob FROM ".$schema.$table_name." FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

$row = oci_fetch_array($s);

var_dump(oci_lob_read($row[0], 10000));

require __DIR__.'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
object(OCILob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
int(4)
bool(true)
int(3)
bool(true)
array(2) {
  [0]=>
  object(OCILob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
  ["BLOB"]=>
  object(OCILob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
bool(true)
string(4) "strt"
string(8) "strtstrt"
Done
