<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create',  'CREATE TABLE test(id int NOT NULL PRIMARY KEY, val VARCHAR(10))');
set_sql('insert1', 'INSERT INTO test VALUES(1, \'A\')'); 
set_sql('insert2', 'INSERT INTO test VALUES(2, \'A\')'); 
set_sql('insert3', 'INSERT INTO test VALUES(3, \'C\')');
set_sql('select',  'SELECT val, id FROM test');


$DB->exec($SQL['create']);
$DB->exec($SQL['insert1']); 
$DB->exec($SQL['insert2']); 
$DB->exec($SQL['insert3']); 

var_dump($DB->query($SQL['select'])->fetchAll(PDO_FETCH_NUM|PDO_FETCH_GROUP));
var_dump($DB->query($SQL['select'])->fetchAll(PDO_FETCH_ASSOC|PDO_FETCH_GROUP));

?>
