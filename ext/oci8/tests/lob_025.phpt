--TEST--
oci_lob_read() tests
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
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

var_dump($blob->size());
var_dump($blob->write(str_repeat("string.", 1000)));
var_dump($blob->size());
oci_commit($c);

$select_sql = "SELECT blob FROM ".$schema.$table_name." FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

var_dump($row = oci_fetch_array($s));

var_dump(oci_lob_read($row[0], 2));
var_dump(oci_lob_read($row[0]));
var_dump(oci_lob_read());
var_dump(oci_lob_eof($row[0]));
var_dump(oci_lob_eof());

unset($row[0]->descriptor);
var_dump(oci_lob_read($row[0],1));
var_dump(oci_lob_eof($row[0]));

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
int(0)
int(7000)
int(7000)
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
string(2) "st"

Warning: oci_lob_read() expects exactly 2 parameters, 1 given in %s on line %d
NULL

Warning: oci_lob_read() expects exactly 2 parameters, 0 given in %s on line %d
NULL
bool(false)

Warning: oci_lob_eof() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: oci_lob_read(): Unable to find descriptor property in %s on line %d
bool(false)

Warning: oci_lob_eof(): Unable to find descriptor property in %s on line %d
bool(false)
Done
