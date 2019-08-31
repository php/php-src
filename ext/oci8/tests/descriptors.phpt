--TEST--
commit connection after destroying the descriptor
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

unset($blob);
unset($statement);

oci_commit($c);

$ora_sql = "SELECT blob FROM ".$schema.$table_name." ";
$statement = oci_parse($c,$ora_sql);
oci_execute($statement, OCI_DEFAULT);

var_dump($row = oci_fetch_assoc($statement));
unset($row['BLOB']);

oci_commit($c);

require __DIR__.'/drop_table.inc';

echo "Done\n";
?>
--EXPECTF--
array(1) {
  ["BLOB"]=>
  object(OCI-Lob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
Done
