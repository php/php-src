<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create', 'CREATE TABLE test(id int PRIMARY KEY, val VARCHAR(10), grp VARCHAR(10))');
set_sql('insert1', 'INSERT INTO test VALUES(1, \'A\', \'Group1\')'); 
set_sql('insert2', 'INSERT INTO test VALUES(2, \'B\', \'Group2\')'); 
set_sql('select1',  'SELECT val, grp FROM test');

$DB->exec($SQL['create']);
$DB->exec($SQL['insert1']);
$DB->exec($SQL['insert2']);

class Test
{
	function __construct($name = 'N/A')
	{
		echo __METHOD__ . "($name)\n";
	}
}

$stmt = $DB->query($SQL['select1'], PDO_FETCH_CLASS, 'Test', array('WOW'));

$it = new IteratorIterator($stmt); /* check if we can convert that thing */

/*** HINT: If YOU plan to do so remember not to call rewind() -> see below ***/

foreach($it as $data)
{
	var_dump($data);
}

$it->next();              /* must be allowed */
var_dump($it->current()); /* must return NULL */
var_dump($it->valid());   /* must return false */

class PDOStatementAggregate extends PDOStatement implements IteratorAggregate
{
	private function __construct()
	{
		echo __METHOD__ . "\n";
		$this->setFetchMode(PDO_FETCH_NUM);   
		/* default fetch mode is BOTH, so we see if the ctor can overwrite that */
	}

	function getIterator()
	{
		echo __METHOD__ . "\n";
		$this->execute();
		return new IteratorIterator($this, 'PDOStatement');
	}
}

$stmt = $DB->prepare($SQL['select1'], array(PDO_ATTR_STATEMENT_CLASS=>array('PDOStatementAggregate')));

foreach($stmt as $data)
{
	var_dump($data);
}

?>
