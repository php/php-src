<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create1',  'CREATE TABLE test(id INT NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(16))');
set_sql('insert1', "INSERT INTO test VALUES(1, 'A', 'A')"); 
set_sql('insert2', "INSERT INTO test VALUES(2, 'B', 'B')"); 
set_sql('insert3', "INSERT INTO test VALUES(3, 'C', 'C')");
set_sql('select1',  'SELECT id, val FROM test');
set_sql('select2',  'SELECT id, val, val2 FROM test');
set_sql('select3',  'SELECT COUNT(*) FROM test');

function countColumns($DB, $action) {
    global $SQL;
    $stmt = $DB->query($SQL[$action]);
    $res = $stmt->columnCount();
    return "Counted $res columns after $action.\n";
}

$DB->exec($SQL['create1']);
$DB->exec($SQL['insert1']); 
$DB->exec($SQL['insert2']); 
$DB->exec($SQL['insert3']); 

echo countColumns($DB, 'select1');
echo countColumns($DB, 'select2');
echo countColumns($DB, 'select3');
?>
