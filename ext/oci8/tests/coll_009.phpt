--TEST--
collections and wrong dates
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

$ora_sql = "CREATE TYPE ".$type_name." AS TABLE OF DATE";
			  
$statement = OCIParse($c,$ora_sql);
OCIExecute($statement);


$coll1 = ocinewcollection($c, $type_name);
$coll2 = ocinewcollection($c, $type_name);

var_dump($coll1->append("2005-07-28"));

var_dump($coll2->assign($coll1));

var_dump($coll2->getElem(0));

echo "Done\n";

require dirname(__FILE__)."/drop_type.inc";

?>
--EXPECTF--
Warning: OCI-Collection::append(): OCI-01861: literal does not match format string in %s on line %d
bool(false)
bool(true)
bool(false)
Done
