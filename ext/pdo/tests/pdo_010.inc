<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create1', 'CREATE TABLE classtypes(id int NOT NULL PRIMARY KEY, name VARCHAR(10) UNIQUE)');
set_sql('insert1', 'INSERT INTO classtypes VALUES(0, \'stdClass\')'); 
set_sql('insert2', 'INSERT INTO classtypes VALUES(1, \'Test1\')'); 
set_sql('insert3', 'INSERT INTO classtypes VALUES(2, \'Test2\')'); 
set_sql('create2', 'CREATE TABLE test(id int NOT NULL PRIMARY KEY, classtype int, val VARCHAR(10), grp VARCHAR(10))');
set_sql('insert4', 'INSERT INTO test VALUES(1, 0, \'A\', \'Group1\')'); 
set_sql('insert5', 'INSERT INTO test VALUES(2, 1, \'B\', \'Group1\')'); 
set_sql('insert6', 'INSERT INTO test VALUES(3, 2, \'C\', \'Group2\')'); 
set_sql('insert7', 'INSERT INTO test VALUES(4, 3, \'D\', \'Group2\')'); 
set_sql('select',  'SELECT classtypes.name, test.grp AS grp, test.id AS id, test.val AS val FROM test LEFT JOIN classtypes ON test.classtype=classtypes.id');

$DB->exec($SQL['create1']);
$DB->exec($SQL['insert1']);
$DB->exec($SQL['insert2']);
$DB->exec($SQL['insert3']);
$DB->exec($SQL['create2']);
$DB->exec($SQL['insert4']);
$DB->exec($SQL['insert5']);
$DB->exec($SQL['insert6']);
$DB->exec($SQL['insert7']);

class Test1
{
	public function __construct()
	{
		echo __METHOD__ . "()\n";
	}
}

class Test2
{
	public function __construct()
	{
		echo __METHOD__ . "()\n";
	}
}

class Test3
{
	public function __construct()
	{
		echo __METHOD__ . "()\n";
	}
}

var_dump($DB->query($SQL['select'])->fetchAll(PDO_FETCH_CLASS|PDO_FETCH_CLASSTYPE|PDO_FETCH_GROUP, 'Test3'));
var_dump($DB->query($SQL['select'])->fetchAll(PDO_FETCH_CLASS|PDO_FETCH_CLASSTYPE|PDO_FETCH_UNIQUE, 'Test3'));

?>
