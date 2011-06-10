--TEST--
collections and nulls (2)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?> 
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

$ora_sql = "DROP TYPE
						".$type_name."
		   ";

$statement = OCIParse($c,$ora_sql);
@OCIExecute($statement);

$ora_sql = "CREATE TYPE ".$type_name." AS TABLE OF VARCHAR(10)";
			  
$statement = OCIParse($c,$ora_sql);
OCIExecute($statement);


$coll1 = ocinewcollection($c, $type_name);

var_dump($coll1->append("string"));
var_dump($coll1->assignElem(0, null));
var_dump($coll1->getElem(0));

echo "Done\n";

require dirname(__FILE__)."/drop_type.inc";

?>
--EXPECT--
bool(true)
bool(true)
NULL
Done
