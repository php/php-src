--TEST--
Test PdoSqlite::createFunction() trampoline callback
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PdoSqlite('sqlite::memory:');

$db->query('CREATE TABLE test_pdo_sqlite_createaggregate_trampoline (a INTEGER, b INTEGER)');

$stmt = $db->query('INSERT INTO test_pdo_sqlite_createaggregate_trampoline VALUES (1, -1), (2, -2), (3, -3), (4, -4), (4, -4)');

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        return strtoupper($arguments[0]);
    }
}

var_dump($db->createFunction('strtoupper', [new TrampolineTest(), 'strtoupper']));

foreach ($db->query('SELECT strtoupper("test")') as $row) {
    var_dump($row);
}

foreach ($db->query('SELECT strtoupper("test")') as $row) {
    var_dump($row);
}

?>
--EXPECT--
bool(true)
Trampoline for strtoupper
array(2) {
  ["strtoupper("test")"]=>
  string(4) "TEST"
  [0]=>
  string(4) "TEST"
}
Trampoline for strtoupper
array(2) {
  ["strtoupper("test")"]=>
  string(4) "TEST"
  [0]=>
  string(4) "TEST"
}
