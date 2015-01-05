--TEST--
PDO_Firebird: prepare/execute/binding
--SKIPIF--
<?php include("skipif.inc"); ?>
<?php function_exists("ibase_query") or die("skip"); ?>
--INI--
ibase.timestampformat=%Y-%m-%d %H:%M:%S
--FILE--
<?php /* $Id$ */

	require("testdb.inc");
    
	$db = new PDO("firebird:dbname=$test_base",$user,$password) or die;

	var_dump($db->getAttribute(PDO::ATTR_CONNECTION_STATUS));

	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

	$db->exec("CREATE TABLE ddl (id SMALLINT NOT NULL PRIMARY KEY, text VARCHAR(32),
		datetime TIMESTAMP DEFAULT '2000-02-12' NOT NULL)");
	$db->exec("INSERT INTO ddl (id,text) VALUES (1,'bla')");
	
	$s = $db->prepare("SELECT * FROM ddl WHERE id=? FOR UPDATE");

	$id = 0;
	$s->bindParam(1,$id);
	$var = null;
	$s->bindColumn("TEXT",$var);
	$id = 1;
	$s->execute();
	$s->setAttribute(PDO::ATTR_CURSOR_NAME, "c");
	
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
bool(true)
int(1)
array(6) {
  ["ID"]=>
  string(1) "1"
  [0]=>
  string(1) "1"
  ["TEXT"]=>
  string(3) "bla"
  [1]=>
  string(3) "bla"
  ["DATETIME"]=>
  string(19) "2000-02-12 00:00:00"
  [2]=>
  string(19) "2000-02-12 00:00:00"
}
string(3) "bla"
int(1)
bool(false)
done
