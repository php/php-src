--TEST--
oci_lob_write() and friends
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
                       ".$schema.$table_name." (id, clob)
                      VALUES (2, empty_clob())
                      RETURNING
                               clob
                      INTO :v_clob ";

$statement = oci_parse($c,$ora_sql);
$clob = oci_new_descriptor($c,OCI_D_LOB);
oci_bind_by_name($statement,":v_clob", $clob,-1,OCI_B_CLOB);
oci_execute($statement, OCI_DEFAULT);

oci_commit($c);  // This will cause subsequent ->write() to fail
$clob->write("data");

require __DIR__.'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
Warning: OCI-Lob::write(): ORA-22990: %s in %s on line 19
Done
