--TEST--
collections and strings
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--ENV--
return "
ORACLE_HOME=".(isset($_ENV['ORACLE_HOME']) ? $_ENV['ORACLE_HOME'] : '')."
NLS_LANG=".(isset($_ENV['NLS_LANG']) ? $_ENV['NLS_LANG'] : '')."
TNS_ADMIN=".(isset($_ENV['TNS_ADMIN']) ? $_ENV['TNS_ADMIN'] : '')."
";
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
$coll2 = ocinewcollection($c, $type_name);

var_dump($coll1->append("string"));
var_dump($coll1->append("string"));

var_dump($coll2->assign($coll1));

var_dump($coll2->getElem(0));

echo "Done\n";

require dirname(__FILE__)."/drop_type.inc";

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(6) "string"
Done
