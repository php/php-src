--TEST--
collections and nulls (2)
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
?>
--FILE--
<?php

require __DIR__."/connect.inc";

error_reporting(E_ALL ^ E_DEPRECATED);

$ora_sql = "DROP TYPE ".$type_name;

$statement = oci_parse($c,$ora_sql);
@oci_execute($statement);

$ora_sql = "CREATE TYPE ".$type_name." AS TABLE OF VARCHAR(10)";

$statement = oci_parse($c,$ora_sql);
oci_execute($statement);


$coll1 = oci_new_collection($c, $type_name);

var_dump($coll1->append("string"));
var_dump($coll1->assignElem(0, null));
var_dump($coll1->getElem(0));

echo "Done\n";

require __DIR__."/drop_type.inc";

?>
--EXPECT--
bool(true)
bool(true)
NULL
Done
