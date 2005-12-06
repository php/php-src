--TEST--
collections and strings (2)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
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

var_dump($coll1->append("striing"));
var_dump($coll1->assignElem(0,"blah"));
var_dump($coll1->getElem(0));

echo "Done\n";

require dirname(__FILE__)."/drop_type.inc";

?>
--EXPECT--
bool(true)
bool(true)
string(4) "blah"
Done
