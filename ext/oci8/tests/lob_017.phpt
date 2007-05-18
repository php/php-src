--TEST--
returning multiple lobs (using persistent connection)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php
	
require dirname(__FILE__).'/connect.inc';

$c = oci_pconnect($user, $password, $dbase);

$drop = "DROP table lob_test";
$statement = oci_parse($c, $drop);
@oci_execute($statement);

$create = "CREATE table lob_test(lob_1 BLOB, lob_2 BLOB)";
$statement = oci_parse($c, $create);
oci_execute($statement);

$init = "INSERT INTO lob_test VALUES(EMPTY_BLOB(), EMPTY_BLOB())";
$statement = oci_parse($c, $init);
oci_execute($statement);

$select = "SELECT * FROM lob_test FOR UPDATE";
$statement = oci_parse($c, $select);
oci_execute($statement, OCI_DEFAULT);

$row = oci_fetch_assoc($statement);

$row['LOB_1']->write("first");
$row['LOB_2']->write("second");

unset($row);

oci_commit($c);

$select = "SELECT * FROM lob_test FOR UPDATE";
$statement = oci_parse($c, $select);
oci_execute($statement, OCI_DEFAULT);

$row = oci_fetch_assoc($statement);

var_dump($row);
var_dump($row['LOB_1']->load());
var_dump($row['LOB_2']->load());

$drop = "DROP table lob_test";
$statement = oci_parse($c, $drop);
@oci_execute($statement);

echo "Done\n";

?>
--EXPECTF--
array(2) {
  ["LOB_1"]=>
  object(OCI-Lob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
  ["LOB_2"]=>
  object(OCI-Lob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
string(5) "first"
string(6) "second"
Done
