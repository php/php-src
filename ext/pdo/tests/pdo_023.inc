<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create1', 'CREATE TABLE test(id INT NOT NULL PRIMARY KEY, val VARCHAR(10))');
set_sql('insert1', 'INSERT INTO test VALUES(0, \'A\')');
set_sql('insert2', 'INSERT INTO test VALUES(1, \'B\')');
set_sql('select1', 'SELECT val, id FROM test');

$DB->test();

var_dump($DB);

$DB->query($SQL['create1']);
$DB->query($SQL['insert1']);
$DB->query($SQL['insert2']);

$stmt = $DB->query($SQL['select1']);

var_dump($stmt);
var_dump($stmt->fetchAll(PDO_FETCH_COLUMN|PDO_FETCH_UNIQUE));

$stmt = NULL;
$DB = NULL;

?>
