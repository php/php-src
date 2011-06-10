--TEST--
oci_collection_assign()
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?> 
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";
require dirname(__FILE__)."/create_type.inc";

$coll1 = oci_new_collection($c, $type_name);
$coll2 = oci_new_collection($c, $type_name);

var_dump($coll1->append(1));

var_dump($coll2->assign($coll1));

var_dump($coll2->getElem(0));

echo "Done\n";

require dirname(__FILE__)."/drop_type.inc";

?>
--EXPECT--
bool(true)
bool(true)
float(1)
Done
