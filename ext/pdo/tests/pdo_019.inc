<?php # vim:ft=php

require_once('pdo.inc');
	
set_sql('create1', 'CREATE TABLE test(idx int NOT NULL PRIMARY KEY, txt VARCHAR(20))');
set_sql('insert1', 'INSERT INTO test VALUES(0, \'String0\')'); 
set_sql('insert2', 'INSERT INTO test VALUES(1, \'String1\')'); 
set_sql('insert3', 'INSERT INTO test VALUES(2, \'String2\')'); 
set_sql('insert4', 'INSERT INTO test VALUES(3, \'String3\')'); 
set_sql('select1', 'SELECT COUNT(*) FROM test'); 
set_sql('select2', 'SELECT idx, txt FROM test ORDER by idx'); 

echo "===INIT===\n";

$DB->exec($SQL['create1']);
var_dump($DB->exec($SQL['insert1']));
var_dump($DB->exec($SQL['insert2']));
var_dump($DB->exec($SQL['insert3']));
var_dump($DB->exec($SQL['insert4']));
var_dump($DB->query($SQL['select1'])->fetchColumn());

$cont = $DB->query($SQL['select2'])->fetchAll(PDO_FETCH_COLUMN|PDO_FETCH_UNIQUE);
var_dump($cont);

echo "===WHILE===\n";

$stmt2 = $DB->prepare($SQL['select2']);
$stmt2->bindColumn('idx', $idx);
$stmt2->bindColumn('txt', $txt);
$stmt2->execute();

while($stmt2->fetch(PDO_FETCH_BOUND)) {
	var_dump(array($idx=>$txt));
}

?>
