<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create1', 'CREATE TABLE test(id int PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(20))');
set_sql('insert1', 'INSERT INTO test VALUES(1, \'A\', \'A2\')'); 
set_sql('insert2', 'INSERT INTO test VALUES(2, \'A\', \'B2\')'); 
set_sql('select1', 'SELECT id, val, val2 FROM test');
set_sql('select2', 'SELECT val, val2 FROM test');


$DB->exec($SQL['create1']);
$DB->exec($SQL['insert1']); 
$DB->exec($SQL['insert2']); 

var_dump($DB->query($SQL['select1'])->fetchAll(PDO_FETCH_COLUMN));
var_dump($DB->query($SQL['select1'])->fetchAll(PDO_FETCH_COLUMN, 2));
var_dump($DB->query($SQL['select1'])->fetchAll(PDO_FETCH_COLUMN|PDO_FETCH_GROUP));
var_dump($DB->query($SQL['select1'])->fetchAll(PDO_FETCH_COLUMN|PDO_FETCH_UNIQUE));
var_dump($DB->query($SQL['select1'])->fetchAll(PDO_FETCH_COLUMN|PDO_FETCH_UNIQUE, 0));
var_dump($DB->query($SQL['select1'])->fetchAll(PDO_FETCH_COLUMN|PDO_FETCH_UNIQUE, 1));
var_dump($DB->query($SQL['select1'])->fetchAll(PDO_FETCH_COLUMN|PDO_FETCH_UNIQUE, 2));
var_dump($DB->query($SQL['select2'])->fetchAll(PDO_FETCH_COLUMN|PDO_FETCH_GROUP));

?>
