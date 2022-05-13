--TEST--
PDO_sqlite: Testing sqliteCreateCollation()
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$db = new PDO('sqlite::memory:');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$db->query('CREATE TABLE IF NOT EXISTS foobar (id INT AUTO INCREMENT, name TEXT)');

$db->query('INSERT INTO foobar VALUES (NULL, "1")');
$db->query('INSERT INTO foobar VALUES (NULL, "2")');
$db->query('INSERT INTO foobar VALUES (NULL, "10")');

$db->sqliteCreateCollation('MYCOLLATE', function($a, $b) { return strnatcmp($a, $b); });

$result = $db->query('SELECT name FROM foobar ORDER BY name COLLATE MYCOLLATE');
foreach ($result as $row) {
    echo $row['name'] . "\n";
}

$result = $db->query('SELECT name FROM foobar ORDER BY name');
foreach ($result as $row) {
  echo $row['name'] . "\n";
}

$db->query('DROP TABLE foobar');

?>
--EXPECT--
1
2
10
1
10
2
