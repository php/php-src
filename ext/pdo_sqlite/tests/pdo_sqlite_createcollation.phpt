--TEST--
PDO_sqlite: Testing sqliteCreateCollation()
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PDO('sqlite::memory:');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$db->query('CREATE TABLE test_pdo_sqlite_createcollation (id INT AUTO INCREMENT, name TEXT)');

$db->query('INSERT INTO test_pdo_sqlite_createcollation VALUES (NULL, "1"), (NULL, "2"), (NULL, "10")');

$db->sqliteCreateCollation('MYCOLLATE', function($a, $b) { return strnatcmp($a, $b); });

$result = $db->query('SELECT name FROM test_pdo_sqlite_createcollation ORDER BY name COLLATE MYCOLLATE');
foreach ($result as $row) {
    echo $row['name'] . "\n";
}

$result = $db->query('SELECT name FROM test_pdo_sqlite_createcollation ORDER BY name');
foreach ($result as $row) {
  echo $row['name'] . "\n";
}

?>
--EXPECT--
1
2
10
1
10
2
