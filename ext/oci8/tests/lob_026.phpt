--TEST--
oci_lob_seek()/rewind()/append()
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

var_dump(oci_lob_write($blob, "test"));
var_dump(oci_lob_rewind());
var_dump(oci_lob_rewind($blob));
var_dump(oci_lob_write($blob, "str"));
var_dump(oci_lob_seek(10, OCI_SEEK_SET));
var_dump(oci_lob_seek($blob, 10, OCI_SEEK_SET));

oci_commit($c);

$select_sql = "SELECT blob FROM ".$schema.$table_name." FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

var_dump($row = oci_fetch_array($s));

var_dump(oci_lob_append());
var_dump(oci_lob_append($blob));
var_dump(oci_lob_append($row[0], $blob));
var_dump(oci_lob_read(10000));
var_dump(oci_lob_read($row[0], 10000));

oci_commit($c);

$select_sql = "SELECT blob FROM ".$schema.$table_name." FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

$row = oci_fetch_array($s);

var_dump(oci_lob_read($row[0], 10000));

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
object(OCI-Lob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
int(4)

Warning: oci_lob_rewind() expects exactly 1 parameter, 0 given in %s on line %d
NULL
bool(true)
int(3)

Warning: oci_lob_seek() expects parameter 1 to be OCI-Lob, integer given in %s on line %d
NULL
bool(true)
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

Warning: oci_lob_append() expects exactly 2 parameters, 0 given in %s on line %d
NULL

Warning: oci_lob_append() expects exactly 2 parameters, 1 given in %s on line %d
NULL
bool(true)

Warning: oci_lob_read() expects exactly 2 parameters, 1 given in %s on line %d
NULL
string(4) "strt"
string(8) "strtstrt"
Done
