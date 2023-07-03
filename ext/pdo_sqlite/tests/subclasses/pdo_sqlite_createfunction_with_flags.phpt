--TEST--
PDO_sqlite: Testing createFunction() with flags
--EXTENSIONS--
pdo_sqlite
--SKIPIF--
<?php
if (!defined('PDO::SQLITE_DETERMINISTIC')) die('skip system sqlite is too old');
?>
--FILE--
<?php
// This test was copied from the pdo_sqlite test for sqliteCreateCollation
$db = new PdoSqlite('sqlite::memory:');
$db->query('CREATE TABLE IF NOT EXISTS foobar (id INT AUTO INCREMENT, name TEXT)');
$db->query('INSERT INTO foobar VALUES (NULL, "PHP")');
$db->query('INSERT INTO foobar VALUES (NULL, "PHP6")');

$db->createFunction('testing', function($v) { return strtolower($v); }, 1, PdoSqlite::DETERMINISTIC);

foreach ($db->query('SELECT testing(name) FROM foobar') as $row) {
    var_dump($row);
}

$db->query('DROP TABLE foobar');

?>
--EXPECT--
array(2) {
  ["testing(name)"]=>
  string(3) "php"
  [0]=>
  string(3) "php"
}
array(2) {
  ["testing(name)"]=>
  string(4) "php6"
  [0]=>
  string(4) "php6"
}
