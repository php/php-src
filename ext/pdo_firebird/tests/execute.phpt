--TEST--
PDO_Firebird: prepare/execute/binding
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php /* $Id$ */

	require("testdb.inc");
    
	$db = new PDO("firebird:dbname=$test_base",$user,$password) or die;
	$db->setAttribute(PDO_ATTR_ERRMODE, PDO_ERRMODE_WARNING);

	$db->exec("CREATE TABLE ddl (id SMALLINT NOT NULL PRIMARY KEY, text VARCHAR(32))");
	$db->exec("INSERT INTO ddl VALUES (1,'bla')");
	
	$s = $db->prepare("SELECT * FROM ddl WHERE id=? FOR UPDATE");

	$s->bindParam(1,$id = 0);
	$s->bindColumn(2,$var = null);
	$id = 1;
	$s->execute();
	$s->setAttribute(PDO_ATTR_CURSOR_NAME, "c");
	
	var_dump($id);

	var_dump($s->fetch());

	var_dump($var);
	
	var_dump($db->exec("UPDATE ddl SET id=2 WHERE CURRENT OF c"));

	var_dump($s->fetch());
	
	unset($s);	
	unset($db);
	echo "done\n";
	
?>
--EXPECT--
int(1)
array(4) {
  ["ID"]=>
  int(1)
  [0]=>
  int(1)
  ["TEXT"]=>
  string(3) "bla"
  [1]=>
  string(3) "bla"
}
string(3) "bla"
int(1)
bool(false)
done
