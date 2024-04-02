--TEST--
PDO_Firebird: ingnore parameter marks in comments
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); 	
?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
require("testdb.inc");

$dbh = getDbConnection();
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

$sql = '
select 1 as n
-- :f
from rdb$database 
where 1=:d and 2=:e	
';
	$query = $dbh->prepare($sql);
	$query->execute(['d' => 1, 'e' => 2]);
	$row = $query->fetch(\PDO::FETCH_OBJ);
	var_dump($row->N);
	unset($query);

	$sql = '
select 1 as n
from rdb$database 
where 1=:d /* and :f = 5 */ and 2=:e	
';
$query = $dbh->prepare($sql);
$query->execute(['d' => 1, 'e' => 2]);
$row = $query->fetch(\PDO::FETCH_OBJ);
var_dump($row->N);
unset($query);

unset($dbh);
echo "done\n";
?>
--EXPECT--
int(1)
int(1)
done
