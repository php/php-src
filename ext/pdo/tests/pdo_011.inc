<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create', 'CREATE TABLE test(id int NOT NULL PRIMARY KEY, val VARCHAR(10), grp VARCHAR(10))');
set_sql('insert1', 'INSERT INTO test VALUES(1, \'A\', \'Group1\')'); 
set_sql('insert2', 'INSERT INTO test VALUES(2, \'B\', \'Group1\')'); 
set_sql('insert3', 'INSERT INTO test VALUES(3, \'C\', \'Group2\')'); 
set_sql('insert4', 'INSERT INTO test VALUES(4, \'D\', \'Group2\')'); 
set_sql('select1',  'SELECT grp, id FROM test');
set_sql('select2',  'SELECT id, val FROM test');

$DB->exec($SQL['create']);
$DB->exec($SQL['insert1']);
$DB->exec($SQL['insert2']);
$DB->exec($SQL['insert3']);
$DB->exec($SQL['insert4']);

class Test1
{
	public function __construct($id, $val)
	{
		echo __METHOD__ . "($id,$val)\n";
		$this->id = $id;
		$this->val = $val;
	}

	static public function factory($id, $val)
	{
		echo __METHOD__ . "($id,$val)\n";
		return new self($id, $val);
	}
}

function test($id,$val='N/A')
{
	echo __METHOD__ . "($id,$val)\n";
	return array($id=>$val);
}

$f = new Test1(0,0);

var_dump($DB->query($SQL['select1'])->fetchAll(PDO_FETCH_FUNC|PDO_FETCH_GROUP, 'test'));
var_dump($DB->query($SQL['select2'])->fetchAll(PDO_FETCH_FUNC, 'test'));
var_dump($DB->query($SQL['select2'])->fetchAll(PDO_FETCH_FUNC, array('Test1','factory')));
var_dump($DB->query($SQL['select2'])->fetchAll(PDO_FETCH_FUNC, array($f, 'factory')));

class DerivedStatement extends PDOStatement
{
	private function __construct($name, $db)
	{
		$this->name = $name;
		echo __METHOD__ . "($name)\n";
	}

	function retrieve($id, $val) {
		echo __METHOD__ . "($id,$val)\n";
		return array($id=>$val);
	}
}

$stmt = $DB->prepare($SQL['select2'], array(PDO_ATTR_STATEMENT_CLASS=>array('DerivedStatement', array('Overloaded', $DB))));
var_dump(get_class($stmt));
$stmt->execute();
var_dump($stmt->fetchAll(PDO_FETCH_FUNC, array($stmt, 'retrieve')));

?>
