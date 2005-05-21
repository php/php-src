<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create',  'CREATE TABLE test(id INT NOT NULL PRIMARY KEY, val VARCHAR(10))');
set_sql('insert1', "INSERT INTO test VALUES(1, 'A')"); 
set_sql('insert2', "INSERT INTO test VALUES(2, 'B')"); 
set_sql('insert3', "INSERT INTO test VALUES(3, 'C')");
set_sql('select',  'SELECT COUNT(*) FROM test');
set_sql('delete',  'DELETE FROM test');

function countRows($DB, $action) {
    global $SQL;
    $stmt = $DB->query($SQL['select']);
    $res = $stmt->fetchColumn();
    return "Counted $res rows after $action.\n";
}

$DB->exec($SQL['create']);
$DB->exec($SQL['insert1']); 
$DB->exec($SQL['insert2']); 
$DB->exec($SQL['insert3']); 

echo countRows($DB, 'insert');

$DB->beginTransaction();
$DB->exec($SQL['delete']); 
echo countRows($DB, 'delete');
$DB->rollBack();

echo countRows($DB, 'rollback');
?>
