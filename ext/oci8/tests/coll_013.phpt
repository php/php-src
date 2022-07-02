--TEST--
collections and correct dates (2)
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__."/connect.inc";

$ora_sql = "DROP TYPE
                        ".$type_name."
           ";

$statement = oci_parse($c,$ora_sql);
@oci_execute($statement);

$ora_sql = "CREATE TYPE ".$type_name." AS TABLE OF DATE";

$statement = oci_parse($c,$ora_sql);
oci_execute($statement);


$coll1 = oci_new_collection($c, $type_name);

var_dump($coll1->append("28-JUL-05"));
var_dump($coll1->assignElem(0,"01-JAN-05"));
var_dump($coll1->getElem(0));

echo "Done\n";

require __DIR__."/drop_type.inc";

?>
--EXPECT--
bool(true)
bool(true)
string(9) "01-JAN-05"
Done
