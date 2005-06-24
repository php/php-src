<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create1',  'CREATE TABLE test(id INT NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(16))');
set_sql('insert1', "INSERT INTO test VALUES(?, ?, ?)"); 
set_sql('select',  'SELECT * FROM test');

$data = array(
    array('10', 'Abc', 'zxy'),
    array('20', 'Def', 'wvu'),
    array('30', 'Ghi', 'tsr'),
);

class PDOStatementX extends PDOStatement
{
    public $dbh;
    
    protected function __construct($dbh)
    {
    	$this->dbh = $dbh;
    	echo __METHOD__ . "()\n";
    }
    
    function __destruct()
    {
    	echo __METHOD__ . "()\n";
    }
}

class PDODatabase extends PDO
{
    function __destruct()
    {
    	echo __METHOD__ . "()\n";
    }
    
    function query($sql)
    {
    	echo __METHOD__ . "()\n";
    	$stmt = $this->prepare($sql, array(PDO_ATTR_STATEMENT_CLASS=>array('PDOStatementx', array($this))));
    	$stmt->setFetchMode(PDO_FETCH_ASSOC);
    	$stmt->execute();
    	return $stmt;
    }
}

$DB = new PDODatabase($CONNECTION, $USER ? $USER : NULL, $PASSWD ? $PASSWD : NULL);
var_dump(get_class($DB));

$DB->setAttribute(PDO_ATTR_ERRMODE, PDO_ERRMODE_WARNING);

$DB->exec($SQL['create1']);

$stmt = $DB->prepare($SQL['insert1']);
var_dump(get_class($stmt));
foreach ($data as $row) {
    $stmt->execute($row);
}

$stmt = $DB->query($SQL['select']);
var_dump(get_class($stmt));
var_dump(get_class($stmt->dbh));

foreach($stmt as $obj) {
	var_dump($obj);
}

?>
