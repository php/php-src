--TEST--
PDO_Firebird: support EXECUTE BLOCK
--SKIPIF--
<?php require('skipif.inc'); 	
?>
--FILE--
<?php
	require("testdb.inc");

	$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

	$sql = '
execute block (a int = :e, b int = :d)
returns (N int, M int)
as
declare z int;
begin
  select 10
  from rdb$database
  into :z;
  
  n = a + b + z;
  m = z * a;
  suspend;
end	
';
	$query = $dbh->prepare($sql);
	$query->execute(['d' => 1, 'e' => 2]);
	$row = $query->fetch(\PDO::FETCH_OBJ);
	var_dump($row->N);
	var_dump($row->M);

	unset($query);
	unset($dbh);
	echo "done\n";

?>
--EXPECT--
int(13)
int(20)
done
