<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create',  'CREATE TABLE test(id int PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(10))');
set_sql('insert1', 'INSERT INTO test VALUES(1, \'A\', \'AA\')'); 
set_sql('insert2', 'INSERT INTO test VALUES(2, \'B\', \'BB\')'); 
set_sql('insert3', 'INSERT INTO test VALUES(3, \'C\', \'CC\')');
set_sql('select',  'SELECT id, val, val2 FROM test');


$DB->exec($SQL['create']);
$DB->exec($SQL['insert1']); 
$DB->exec($SQL['insert2']); 
$DB->exec($SQL['insert3']); 

class TestBase
{
	public $id;
	protected $val;
	private $val2;
}

class TestDerived extends TestBase
{
	protected $row;

	public function __construct(&$row)
	{
		echo __METHOD__ . "($row,{$this->id})\n";
		$this->row = $row++;
	}
}

var_dump($DB->query($SQL['select'])->fetchAll(PDO_FETCH_CLASS));
var_dump($DB->query($SQL['select'])->fetchAll(PDO_FETCH_CLASS, 'TestBase'));
var_dump($DB->query($SQL['select'])->fetchAll(PDO_FETCH_CLASS, 'TestDerived', array(0)));
?>
