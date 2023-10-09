--TEST--
oci_execute() segfault after repeated bind of LOB descriptor
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';
require __DIR__.'/create_table.inc';

$ora_sql = "INSERT INTO
                       ".$schema.$table_name." (blob, clob)
                      VALUES (empty_blob(), empty_clob())
                      RETURNING
                               blob
                      INTO :v_blob ";

$s = oci_parse($c, $ora_sql);
$blob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ":v_blob", $blob, -1, OCI_B_BLOB);
oci_execute($s, OCI_DEFAULT);
var_dump($blob->save("some binary data"));

oci_bind_by_name($s, ":v_blob", $blob, -1, OCI_B_BLOB);
oci_execute($s, OCI_DEFAULT);
var_dump($blob->save("some more binary data"));

$query = 'SELECT blob, DBMS_LOB.GETLENGTH(blob) FROM '.$schema.$table_name.' ORDER BY 2';

$s = oci_parse ($c, $query);
oci_execute($s, OCI_DEFAULT);

while ($arr = oci_fetch_assoc($s)) {
    $result = $arr['BLOB']->load();
    var_dump($result);
}

require __DIR__.'/drop_table.inc';

echo "Done\n";

?>
--EXPECT--
bool(true)
bool(true)
string(16) "some binary data"
string(21) "some more binary data"
Done
