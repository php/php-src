--TEST--
various tests with wrong param count
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
$blob = oci_new_descriptor($c,OCI_D_LOB,1,2,3);
$blob = oci_new_descriptor($c);
oci_bind_by_name($statement,":v_blob", $blob,-1,OCI_B_BLOB,4);
oci_bind_by_name($statement,":v_blob", $blob,-1,OCI_B_BLOB,4,5);
oci_bind_by_name($statement,":v_blob", $blob,-1);
oci_bind_by_name($statement,":v_blob", $blob);
oci_bind_by_name($statement,":v_blob");
oci_bind_by_name($statement);
oci_execute($statement, OCI_DEFAULT);

var_dump($blob);

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
Warning: oci_new_descriptor() expects at most 2 parameters, 5 given in %slob_015.php on line %d

Warning: oci_bind_by_name() expects at most 5 parameters, 6 given in %slob_015.php on line %d

Warning: oci_bind_by_name() expects at most 5 parameters, 7 given in %slob_015.php on line %d

Notice: Object of class OCI-Lob to string conversion in %slob_015.php on line %d

Warning: oci_bind_by_name() expects at least 3 parameters, 2 given in %slob_015.php on line %d

Warning: oci_bind_by_name() expects at least 3 parameters, 1 given in %slob_015.php on line %d

Warning: oci_execute(): ORA-00932: inconsistent datatypes: expected NUMBER got BLOB in %slob_015.php on line %d
string(6) "Object"
Done
