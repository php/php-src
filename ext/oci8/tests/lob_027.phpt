--TEST--
oci_lob_truncate()
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';
require dirname(__FILE__).'/create_table.inc';

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

var_dump($blob);

$str = "this is a biiiig faaat test string. why are you reading it, I wonder? =)";
var_dump($blob->write($str));

oci_commit($c);

$select_sql = "SELECT blob FROM ".$schema.$table_name." FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

var_dump($row = oci_fetch_array($s));
oci_commit($c);

for ($i = 5; $i >= 0; $i--) {

	$select_sql = "SELECT blob FROM ".$schema.$table_name." FOR UPDATE";
	$s = oci_parse($c, $select_sql);
	oci_execute($s, OCI_DEFAULT);

	$row = oci_fetch_array($s);
	var_dump($row['BLOB']->load());
	var_dump($row['BLOB']->truncate(($i-1)*10));

	oci_commit($c);
}

$select_sql = "SELECT blob FROM ".$schema.$table_name." FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

$row = oci_fetch_array($s);
var_dump($row['BLOB']->load());
var_dump($row['BLOB']->truncate(-1));
var_dump($row['BLOB']->truncate(0));

oci_commit($c);

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
object(OCI-Lob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
int(72)
array(2) {
  [0]=>
  object(OCI-Lob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
  ["BLOB"]=>
  object(OCI-Lob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
string(72) "this is a biiiig faaat test string. why are you reading it, I wonder? =)"
bool(true)
string(40) "this is a biiiig faaat test string. why "
bool(true)
string(30) "this is a biiiig faaat test st"
bool(true)
string(20) "this is a biiiig faa"
bool(true)
string(10) "this is a "
bool(true)
string(0) ""

Warning: OCI-Lob::truncate(): Length must be greater than or equal to zero in %s on line %d
bool(false)
string(0) ""

Warning: OCI-Lob::truncate(): Length must be greater than or equal to zero in %s on line %d
bool(false)
bool(true)
Done
