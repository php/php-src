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

$db->sqliteCreateCollation('MYCOLLATEBAD', function($a, $b) { return $a; });

try {
	$db->query('SELECT name FROM test_pdo_sqlite_createcollation ORDER BY name COLLATE MYCOLLATEBAD');
} catch (\TypeError $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
Deprecated: Method PDO::sqliteCreateCollation() is deprecated since 8.5, use Pdo\Sqlite::createCollation() instead in %s on line %d
1
2
10
1
10
2

Deprecated: Method PDO::sqliteCreateCollation() is deprecated since 8.5, use Pdo\Sqlite::createCollation() instead in %s on line %d
PDO::query(): Return value of the collation callback must be of type int, string returned
