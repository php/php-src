--TEST--
various oci_lob_write() error messages
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
                       ".$schema.$table_name." (id, blob)
                      VALUES (2, empty_blob())
                      RETURNING
                               blob
                      INTO :v_blob ";

$statement = oci_parse($c,$ora_sql);
$blob = oci_new_descriptor($c,OCI_D_LOB);

$blob->save("");

oci_bind_by_name($statement,":v_blob", $blob,-1,OCI_B_BLOB);
oci_execute($statement, OCI_DEFAULT);

var_dump($blob->save(""));
var_dump($blob->save("data", 100));

require __DIR__.'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
Warning: OCILob::save(): OCI_INVALID_HANDLE in %s on line %d
bool(true)
bool(true)
Done
