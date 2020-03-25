--TEST--
various tests with wrong param count
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
$blob = oci_new_descriptor($c,OCI_D_LOB,1,2,3);
$blob = oci_new_descriptor($c);
$int = 1;
oci_bind_by_name($statement,":v_blob", $blob,-1,OCI_B_BLOB,4);
oci_bind_by_name($statement,":v_blob", $blob,-1,OCI_B_BLOB,4,5);
oci_bind_by_name($statement,":v_blob", $int,-1);
oci_bind_by_name($statement,":v_blob", $int);
oci_bind_by_name($statement,":v_blob");
oci_bind_by_name($statement);
oci_execute($statement, OCI_DEFAULT);

var_dump($blob);

require __DIR__.'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
Warning: oci_new_descriptor() expects at most 2 parameters, 5 given in %s on line %d

Warning: oci_bind_by_name() expects at most 5 parameters, 6 given in %s on line %d

Warning: oci_bind_by_name() expects at most 5 parameters, 7 given in %s on line %d

Warning: oci_bind_by_name() expects at least 3 parameters, 2 given in %s on line %d

Warning: oci_bind_by_name() expects at least 3 parameters, 1 given in %s on line %d
object(OCI-Lob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
Done
