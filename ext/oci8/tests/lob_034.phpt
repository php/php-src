--TEST--
lob buffering - 2
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

var_dump($blob->getBuffering());
var_dump($blob->setBuffering(false));
var_dump($blob->setBuffering(false));
var_dump($blob->setBuffering(true));
var_dump($blob->setBuffering(true));
var_dump($blob->flush());
var_dump($blob->flush(0));
var_dump($blob->flush(-1));

oci_commit($c);

require __DIR__.'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

Warning: OCILob::flush(): Invalid flag value: -1 in %s on line %d
bool(false)
Done
