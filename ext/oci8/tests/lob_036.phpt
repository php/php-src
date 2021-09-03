--TEST--
Exercise cleanup code when LOB buffering is on
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

$s = oci_parse($c,$ora_sql);
$blob = oci_new_descriptor($c,OCI_DTYPE_LOB);


oci_bind_by_name($s,":v_blob", $blob,-1,OCI_B_BLOB);
oci_execute($s, OCI_DEFAULT);

var_dump($blob->write("test"));
var_dump($blob->setBuffering(true));
var_dump($blob->write("test"));

$blob = null;

require __DIR__.'/drop_table.inc';

echo "Done\n";

?>
--EXPECT--
int(4)
bool(true)
int(4)
Done
