--TEST--
oci_lob_copy()
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
                       ".$schema.$table_name." (id, blob)
                      VALUES (1, empty_blob())
                      RETURNING
                               blob
                      INTO :v_blob ";

$statement = oci_parse($c,$ora_sql);
$blob = oci_new_descriptor($c,OCI_D_LOB);
oci_bind_by_name($statement,":v_blob", $blob,-1,OCI_B_BLOB);
oci_execute($statement, OCI_DEFAULT);

var_dump($blob->write("some string here. string, I said"));
oci_commit($c);

$ora_sql = "INSERT INTO
                       ".$schema.$table_name." (id, blob)
                      VALUES (2, empty_blob())
                      RETURNING
                               blob
                      INTO :v_blob ";

$statement = oci_parse($c,$ora_sql);
$blob = oci_new_descriptor($c,OCI_D_LOB);
oci_bind_by_name($statement,":v_blob", $blob,-1,OCI_B_BLOB);
oci_execute($statement, OCI_DEFAULT);

oci_commit($c);

$select_sql = "SELECT blob FROM ".$schema.$table_name." WHERE id = 1";
$s = oci_parse($c, $select_sql);
oci_execute($s);

$row1 = oci_fetch_array($s);

$select_sql = "SELECT blob FROM ".$schema.$table_name." WHERE id = 2 FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

$row2 = oci_fetch_array($s);

var_dump(oci_lob_copy($row2[0], $row1[0]));
var_dump($row1[0]->read(100));

oci_commit($c);

$select_sql = "SELECT blob FROM ".$schema.$table_name." WHERE id = 2 FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

var_dump($row2 = oci_fetch_array($s, OCI_RETURN_LOBS));

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECT--
int(32)
bool(true)
string(32) "some string here. string, I said"
array(2) {
  [0]=>
  string(32) "some string here. string, I said"
  ["BLOB"]=>
  string(32) "some string here. string, I said"
}
Done
