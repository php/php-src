--TEST--
Test PdoSqlite::createCollation() trampoline callback
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PdoSqlite('sqlite::memory:');

$db->query('CREATE TABLE test_pdo_sqlite_createcollation_trampoline (s VARCHAR(4))');

$stmt = $db->query('INSERT INTO test_pdo_sqlite_createcollation_trampoline VALUES ("a1"), ("a10"), ("a2")');

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        return strnatcmp(...$arguments);
    }
}

var_dump($db->createCollation('NAT', [new TrampolineTest(), 'NAT']));

echo "default\n";
foreach ($db->query('SELECT s FROM test_pdo_sqlite_createcollation_trampoline ORDER BY s') as $row) {
    var_dump($row);
}

echo "natural\n";
foreach ($db->query('SELECT s FROM test_pdo_sqlite_createcollation_trampoline ORDER BY s COLLATE NAT') as $row) {
    var_dump($row);
}

?>
--EXPECT--
bool(true)
default
array(2) {
  ["s"]=>
  string(2) "a1"
  [0]=>
  string(2) "a1"
}
array(2) {
  ["s"]=>
  string(3) "a10"
  [0]=>
  string(3) "a10"
}
array(2) {
  ["s"]=>
  string(2) "a2"
  [0]=>
  string(2) "a2"
}
natural
Trampoline for NAT
Trampoline for NAT
array(2) {
  ["s"]=>
  string(2) "a1"
  [0]=>
  string(2) "a1"
}
array(2) {
  ["s"]=>
  string(2) "a2"
  [0]=>
  string(2) "a2"
}
array(2) {
  ["s"]=>
  string(3) "a10"
  [0]=>
  string(3) "a10"
}
