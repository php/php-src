<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create',  'CREATE TABLE test(id CHAR(1) PRIMARY KEY, val VARCHAR(10))');
set_sql('insert1', 'INSERT INTO test VALUES(\'A\', \'A\')'); 
set_sql('insert2', 'INSERT INTO test VALUES(\'B\', \'A\')'); 
set_sql('insert3', 'INSERT INTO test VALUES(\'C\', \'C\')');
set_sql('select',  'SELECT val, id FROM test');

$DB->exec($SQL['create']);
$DB->exec($SQL['insert1']); 
$DB->exec($SQL['insert2']); 
$DB->exec($SQL['insert3']); 

var_dump($DB->query($SQL['select'])->fetchAll(PDO_FETCH_NUM|PDO_FETCH_UNIQUE));
// check that repeated first columns overwrite existing array elements

?>
