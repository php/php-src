<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create', 'CREATE TABLE test(id int NOT NULL PRIMARY KEY, val VARCHAR(10), grp VARCHAR(10))');
set_sql('insert1', 'INSERT INTO test VALUES(1, \'A\', \'Group1\')'); 
set_sql('insert2', 'INSERT INTO test VALUES(2, \'B\', \'Group2\')'); 
set_sql('select1',  'SELECT val, grp FROM test');

$DB->exec($SQL['create']);
$DB->exec($SQL['insert1']);
$DB->exec($SQL['insert2']);

$stmt = $DB->query($SQL['select1'], PDO_FETCH_NUM);
var_dump($stmt->fetchAll());

class Test
{
	function __construct($name = 'N/A')
	{
		echo __METHOD__ . "($name)\n";
	}
}

$stmt = $DB->query($SQL['select1'], PDO_FETCH_CLASS, 'Test');
var_dump($stmt->fetchAll());

$stmt = $DB->query($SQL['select1'], PDO_FETCH_NUM);
$stmt->setFetchMode(PDO_FETCH_CLASS, 'Test', array('Changed'));
var_dump($stmt->fetchAll());

?>
