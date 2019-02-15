--TEST--
oci_lob_import()/read()
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
var_dump($blob->seek(10, OCI_SEEK_CUR));
var_dump($blob->import(dirname(__FILE__)."/lob_009.txt"));
var_dump($blob->import());
var_dump(oci_lob_import($blob));
var_dump(oci_lob_import($blob, dirname(__FILE__)."/lob_009.txt"));
unset($blob->descriptor);
var_dump(oci_lob_import($blob, dirname(__FILE__)."/lob_009.txt"));
oci_commit($c);

$select_sql = "SELECT blob FROM ".$schema.$table_name." FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

var_dump($row = oci_fetch_array($s));

while (!$row[0]->eof()) {
	var_dump(str_replace("\r", "", $row[0]->read(1024)));
}

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
object(OCI-Lob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
bool(true)
bool(true)

Warning: OCI-Lob::import() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: oci_lob_import() expects exactly 2 parameters, 1 given in %s on line %d
NULL
bool(true)

Warning: oci_lob_import(): Unable to find descriptor property in %s on line %d
bool(false)
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
string(43) "this
is
a 
test
file for
test lob_009.phpt
"
Done
