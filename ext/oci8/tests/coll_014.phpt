--TEST--
collections and strings (2)
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

$statement = OCIParse($c,$ora_sql);
@OCIExecute($statement);

$ora_sql = "CREATE TYPE ".$type_name." AS TABLE OF VARCHAR(10)";

$statement = OCIParse($c,$ora_sql);
OCIExecute($statement);


$coll1 = ocinewcollection($c, $type_name);

var_dump($coll1->append("striing"));
var_dump($coll1->assignElem(0,"blah"));
var_dump($coll1->getElem(0));

echo "Done\n";

require __DIR__."/drop_type.inc";

?>
--EXPECT--
bool(true)
bool(true)
string(4) "blah"
Done
