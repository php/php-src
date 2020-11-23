--TEST--
collections and wrong dates
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
$coll2 = oci_new_collection($c, $type_name);

var_dump($coll1->append("2005-07-28"));

var_dump($coll2->assign($coll1));

var_dump($coll2->getElem(0));

echo "Done\n";

require __DIR__."/drop_type.inc";

?>
--EXPECTF--
Warning: OCICollection::append(): OCI-01861: literal does not match format string in %s on line %d
bool(false)
bool(true)
bool(false)
Done
