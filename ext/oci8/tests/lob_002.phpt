--TEST--
oci_lob_write() and friends (with errors)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "drop table lob_002_tab",
    "create table lob_002_tab (id number, b1 BLOB)",
);

oci8_test_sql_execute($c, $stmtarray);

$statement = oci_parse($c, "insert into lob_002_tab (id, b1) values (1, empty_blob()) returning b1 INTO :v_blob ");
$blob = oci_new_descriptor($c,OCI_D_LOB);
oci_bind_by_name($statement,":v_blob", $blob,-1,OCI_B_BLOB);
oci_execute($statement, OCI_DEFAULT);

var_dump($blob);

var_dump($blob->write("test", -1));
var_dump($blob->write("test", 1000000));
var_dump($blob->write(str_repeat("test", 10000), 1000000));
var_dump($blob->tell());
var_dump($blob->flush());

oci_commit($c);

$select_sql = "select b1 from lob_002_tab where id = 1";
$s = oci_parse($c, $select_sql);
oci_execute($s);

$row = oci_fetch_array($s, OCI_RETURN_LOBS);

var_dump(strlen($row[0]));

// Cleanup

$stmtarray = array(
    "drop table lob_002_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECTF--
object(OCILob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
int(0)
int(4)
int(40000)
int(40004)
bool(false)
int(40004)
