<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create', 'CREATE TABLE test(id int NOT NULL PRIMARY KEY, val VARCHAR(10), grp VARCHAR(10))');
set_sql('insert1', 'INSERT INTO test VALUES(1, \'A\', \'Group1\')'); 
set_sql('insert2', 'INSERT INTO test VALUES(2, \'B\', \'Group2\')'); 
set_sql('select1',  'SELECT val, grp FROM test');

$DB->exec($SQL['create']);
$DB->exec($SQL['insert1']);
$DB->exec($SQL['insert2']);

foreach($DB->query($SQL['select1'], PDO_FETCH_NUM) as $data)
{
	var_dump($data);
}

class Test
{
	function __construct($name = 'N/A')
	{
		echo __METHOD__ . "($name)\n";
	}
}

foreach($DB->query($SQL['select1'], PDO_FETCH_CLASS, 'Test') as $data)
{
	var_dump($data);
}

$stmt = $DB->query($SQL['select1'], PDO_FETCH_CLASS, 'Test', array('WOW'));

foreach($stmt as $data)
{
	var_dump($data);
}

?>
