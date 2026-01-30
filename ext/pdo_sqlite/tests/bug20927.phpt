--TEST--
PDO_SQLITE: fseek() with PHP_INT_MIN on sqlite blob stream (UBSan regression test)
--EXTENSIONS--
pdo
pdo_sqlite
--FILE--
<?php
$db = new Pdo\Sqlite('sqlite::memory:'); 
$db->exec('CREATE TABLE test (id STRING, data BLOB)'); 
$insert_stmt = $db->prepare("INSERT INTO test (id, data) VALUES (?, ?)"); 
$insert_stmt->bindValue(1, 'a', PDO::PARAM_STR); 
$insert_stmt->bindValue(2, 'TEST TEST', PDO::PARAM_LOB); 
$insert_stmt->execute(); 
$stream = $db->openBlob('test', 'data', 1); 
var_dump(fseek($stream, PHP_INT_MIN, SEEK_END));  
?>
--EXPECT--
int(-1)