--TEST--
collections and negative/too big element indexes
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

$ora_sql = "DROP TYPE
                        ".$type_name."
           ";

$statement = oci_parse($c,$ora_sql);
@oci_execute($statement);

$ora_sql = "CREATE TYPE ".$type_name." AS TABLE OF NUMBER";

$statement = oci_parse($c,$ora_sql);
oci_execute($statement);


$coll1 = oci_new_collection($c, $type_name);

var_dump($coll1->append(1));
var_dump($coll1->assignElem(-1,2345));
var_dump($coll1->assignElem(5000,2345));
var_dump($coll1->getElem(-1));
var_dump($coll1->getElem(-100));
var_dump($coll1->getElem(500));

echo "Done\n";

require __DIR__."/drop_type.inc";

?>
--EXPECTF--
bool(true)

Warning: OCICollection::assignElem(): OCI-22165: given index [%d] must be in the range of %s to [0] in %s on line %d
bool(false)

Warning: OCICollection::assignElem(): OCI-22165: given index [5000] must be in the range of %s to [0] in %s on line %d
bool(false)
bool(false)
bool(false)
bool(false)
Done
