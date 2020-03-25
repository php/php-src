--TEST--
Bug #36010 (Crash when executing SQL statement with lob parameter twice)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

function f($conn)
{
    $sql =  "begin :p_clob := 'lob string'; end;";
    $stid = oci_parse($conn, $sql);
    $clob = oci_new_descriptor($conn, OCI_D_LOB);
    oci_bind_by_name($stid, ":p_clob", $clob, -1, OCI_B_CLOB);
    $r = oci_execute($stid, OCI_DEFAULT);
}

f($c);
f($c);

echo "Done\n";

?>
--EXPECT--
Done
