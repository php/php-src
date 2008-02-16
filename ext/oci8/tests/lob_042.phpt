--TEST--
Check various LOB error messages
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

// test some LOB error messages
	
require(dirname(__FILE__).'/connect.inc');
require(dirname(__FILE__).'/create_table.inc');

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

var_dump($blob->writeTemporary("test", OCI_D_LOB));

$str = "string";
var_dump($blob->write($str));
var_dump($blob->truncate(1));
var_dump($blob->truncate(1));
var_dump($blob->truncate(2));
var_dump($blob->truncate(-1));
var_dump($blob->read(2));

var_dump($blob->import("does_not_exist"));
var_dump($blob->saveFile("does_not_exist"));

require(dirname(__FILE__).'/drop_table.inc');
						 
echo "Done\n";
						 
?>
--EXPECTF--
object(OCI-Lob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}

Warning: OCI-Lob::writetemporary(): Invalid temporary lob type: %d in %s on line %d
bool(false)
int(6)
bool(true)
bool(true)

Warning: OCI-Lob::truncate(): Size must be less than or equal to the current LOB size in %s on line %d
bool(false)

Warning: OCI-Lob::truncate(): Length must be greater than or equal to zero in %s on line %d
bool(false)

Warning: OCI-Lob::read(): Offset must be less than size of the LOB in %s on line %d
bool(false)

Warning: OCI-Lob::import(): Can't open file %s in %s on line %d
bool(false)

Warning: OCI-Lob::savefile(): Can't open file %s in %s on line %d
bool(false)
Done
