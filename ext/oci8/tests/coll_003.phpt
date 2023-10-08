--TEST--
collection methods
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
require __DIR__."/create_type.inc";

$coll1 = oci_new_collection($c, $type_name);

var_dump($coll1->size());
var_dump($coll1->max());
var_dump($coll1->trim(3));
var_dump($coll1->append(1));
var_dump($coll1->getElem(0));
var_dump($coll1->assignElem(0,2));

echo "Done\n";

require __DIR__."/drop_type.inc";

?>
--EXPECTF--
int(0)
int(0)

Warning: OCICollection::trim(): OCI-22167: given trim size [3] must be less than or equal to [0] in %s on line %d
bool(false)
bool(true)
float(1)
bool(true)
Done
