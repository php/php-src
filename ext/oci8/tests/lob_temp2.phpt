--TEST--
Writing temporary lob before binding
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');
require(dirname(__FILE__).'/create_table.inc');

$ora_sql = "INSERT INTO ".$schema.$table_name." (clob) VALUES (:v_clob)";

$clob = oci_new_descriptor($c, OCI_D_LOB);
var_dump($clob->writeTemporary("test"));

$statement = oci_parse($c, $ora_sql);
oci_bind_by_name($statement, ":v_clob", $clob, -1, OCI_B_CLOB);
oci_execute($statement, OCI_DEFAULT);

$s = oci_parse($c, "select clob from ". $schema.$table_name);
oci_execute($s);
oci_fetch_all($s, $res);
var_dump($res);

?>
===DONE===
--EXPECT--
bool(true)
array(1) {
  ["CLOB"]=>
  array(1) {
    [0]=>
    string(4) "test"
  }
}
===DONE===
