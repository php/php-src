<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create',  'CREATE TABLE test(id int PRIMARY KEY, val VARCHAR(10))');
set_sql('insert1', 'INSERT INTO test VALUES(1, \'A\')'); 
set_sql('insert2', 'INSERT INTO test VALUES(2, \'B\')'); 
set_sql('insert3', 'INSERT INTO test VALUES(3, \'C\')');
set_sql('select',  'SELECT * FROM test');


$DB->exec($SQL['create']);
$DB->exec($SQL['insert1']); 
$DB->exec($SQL['insert2']); 
$DB->exec($SQL['insert3']); 

$stmt = $DB->query($SQL['select']);

var_dump($stmt->fetchAll(PDO_FETCH_OBJ));
?>
