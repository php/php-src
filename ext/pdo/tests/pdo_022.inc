<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create1',  'CREATE TABLE test(id INT NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(16))');
set_sql('insert1', "INSERT INTO test VALUES(?, ?, ?)"); 
set_sql('insert2', "INSERT INTO test VALUES(:first, :second, :third)"); 
set_sql('select1',  'SELECT id, val, val2 FROM test');
set_sql('select2',  'SELECT COUNT(*) FROM test');

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

// Retrieve column metadata for a result set returned by explicit SELECT
$select = $DB->query($SQL['select1']);
$meta = $select->getColumnMeta(0);
var_dump($meta);
$meta = $select->getColumnMeta(1);
var_dump($meta);
$meta = $select->getColumnMeta(2);
var_dump($meta);

// Retrieve column metadata for a result set returned by a function
$select = $DB->query($SQL['select2']);
$meta = $select->getColumnMeta(0);
var_dump($meta);
?>
