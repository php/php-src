--TEST--
oci_lob_write() and friends (with errors)
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

var_dump($blob);

var_dump($blob->write("test", -1));
var_dump($blob->write("test", "str"));
var_dump($blob->write("test", 1000000));
var_dump($blob->write(str_repeat("test", 10000), 1000000));
var_dump($blob->tell());
var_dump($blob->seek("str", -5));
var_dump($blob->flush());

oci_commit($c);

$select_sql = "SELECT blob FROM ".$schema.$table_name."";
$s = oci_parse($c, $select_sql);
oci_execute($s);

$row = oci_fetch_array($s, OCI_RETURN_LOBS);

var_dump(strlen($row[0]));


require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
object(OCI-Lob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
int(0)

Warning: OCI-Lob::write() expects parameter 2 to be long, string given in %slob_002.php on line %d
NULL
int(4)
int(40000)
int(40004)

Warning: OCI-Lob::seek() expects parameter 1 to be long, string given in %slob_002.php on line %d
NULL
bool(false)
int(40004)
Done
