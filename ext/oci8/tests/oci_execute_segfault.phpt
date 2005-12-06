--TEST--
oci_execute() segfault after repeated bind
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php
	
require dirname(__FILE__).'/connect.inc';
require dirname(__FILE__).'/create_table.inc';

$ora_sql = "INSERT INTO
                       ".$table_name." (blob, clob)
                      VALUES (empty_blob(), empty_clob())
                      RETURNING
                               blob
                      INTO :v_blob ";

$s = oci_parse($c,$ora_sql);
$blob = oci_new_descriptor($c,OCI_D_LOB);
oci_bind_by_name($s,":v_blob", $blob,-1,OCI_B_BLOB);
oci_execute($s);

oci_bind_by_name($s,":v_blob", $blob,-1,OCI_B_BLOB);
oci_execute($s);

echo "Done\n";

?>
--EXPECT--
Done
