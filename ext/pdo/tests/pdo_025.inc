<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create1',  'CREATE TABLE test(id INT NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(16))');
set_sql('insert1', "INSERT INTO test VALUES(?, ?, ?)"); 
set_sql('select',  'SELECT * FROM test');

$data = array(
    array('10', 'Abc', 'zxy'),
    array('20', 'Def', 'wvu'),
    array('30', 'Ghi', 'tsr'),
    array('40', 'Jkl', 'qpo'),
    array('50', 'Mno', 'nml'),
    array('60', 'Pqr', 'kji'),
);

$DB->exec($SQL['create1']);

// Insert using question mark placeholders
$stmt = $DB->prepare($SQL['insert1']);
foreach ($data as $row) {
    $stmt->execute($row);
}

class Test {
	public $id, $val, $val2;
}

$stmt = $DB->prepare($SQL['select']);
$stmt->setFetchMode(PDO_FETCH_INTO, new Test);
$stmt->execute();

foreach($stmt as $obj) {
	var_dump($obj);
}

echo "===FAIL===\n";

class Fail {
	protected $id;
	public $val, $val2;
}

$stmt->setFetchMode(PDO_FETCH_INTO, new Fail);
$stmt->execute();

foreach($stmt as $obj) {
	var_dump($obj);
}
?>
