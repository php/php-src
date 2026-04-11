--TEST--
SQLite3 DQS
--EXTENSIONS--
sqlite3
--SKIPIF--
<?php
require_once(__DIR__ . '/new_db.inc');
try {
    $db->enableExceptions(true);
    $db->exec('SELECT "test"');
} catch (\SQLite3Exception) {
    die('skip SQLite is lacking DQS');
}
?>
--FILE--
<?php
require_once(__DIR__ . '/new_db.inc');
$db->exec('CREATE TABLE test (s1 VARCHAR(255), s2 VARCHAR(255))');
$db->exec('INSERT INTO test VALUES (\'test\', "test")');
var_dump($db->prepare('SELECT * FROM test')->execute()->fetchArray(SQLITE3_ASSOC));
?>
--EXPECT--
array(2) {
  ["s1"]=>
  string(4) "test"
  ["s2"]=>
  string(4) "test"
}
