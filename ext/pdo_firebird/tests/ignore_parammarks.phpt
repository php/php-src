--TEST--
PDO_Firebird: ingnore parameter marks in comments
--SKIPIF--
<?php require('skipif.inc'); 	
?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php
	require("testdb.inc");

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
