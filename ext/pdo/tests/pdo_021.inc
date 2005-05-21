<?php # vim:ft=php

require_once('pdo.inc');

set_sql('create1',  'CREATE TABLE test(id INT NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(16))');
set_sql('insert1', "INSERT INTO test VALUES(?, ?, ?)"); 
set_sql('insert2', "INSERT INTO test VALUES(:first, :second, :third)"); 
set_sql('select',  'SELECT COUNT(*) FROM test');

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

$select = $DB->query($SQL['select']);
$num = $select->fetchColumn();
echo 'There are ' . $num . " rows in the table.\n";

// Insert using named parameters
$stmt2 = $DB->prepare($SQL['insert2']);
foreach ($data as $row) {
    $stmt2->execute(array(':first'=>($row[0] + 5), ':second'=>$row[1], 
        ':third'=>$row[2]));
}

$select = $DB->query($SQL['select']);
$num = $select->fetchColumn();
echo 'There are ' . $num . " rows in the table.\n";

?>
