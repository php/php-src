--TEST--
PDO_Firebird: DDL/transactions
--SKIPIF--
<?php include("skipif.inc"); ?>
<?php function_exists("ibase_query") or die("skip"); ?>
--FILE--
<?php

	require("testdb.inc");

	$db = new PDO("firebird:dbname=$test_base",$user,$password) or die;
	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

	$db->exec("CREATE TABLE ddl (id INT NOT NULL PRIMARY KEY, text BLOB SUB_TYPE 1)");
	$db->exec("CREATE GENERATOR gen_ddl_id");
	$db->exec("CREATE TRIGGER ddl_bi FOR ddl BEFORE INSERT AS
		BEGIN IF (NEW.id IS NULL) THEN NEW.id=GEN_ID(gen_ddl_id,1); END");

	$db->setAttribute(PDO::ATTR_AUTOCOMMIT,0);

	$db->beginTransaction();
	var_dump($db->exec("INSERT INTO ddl (text) VALUES ('bla')"));
	var_dump($db->exec("UPDATE ddl SET text='blabla'"));
	$db->rollback();

	$db->beginTransaction();
	var_dump($db->exec("DELETE FROM ddl"));
	$db->commit();

	unset($db);
	echo "done\n";

?>
--EXPECT--
int(1)
int(1)
int(0)
done
