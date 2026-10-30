--TEST--
PDO_sqlite: Parser custom syntax
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$filename = __DIR__ . DIRECTORY_SEPARATOR . "pdo_sqlite_parser.db";

// Default open flag is read-write|create
$db = new PDO('sqlite:' . $filename, null, null, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);

$table = 'pdo_sqlite_parser';

$db->exec("CREATE TABLE {$table} (`a``?` int NOT NULL)");
$db->exec("INSERT INTO {$table} VALUES (1)");

// No parameters
$queries = [
    "SELECT * FROM {$table}",
    "SELECT * FROM {$table} -- ?",
    "SELECT * FROM {$table} /* ? */",
];

foreach ($queries as $k => $query) {
    $stmt = $db->prepare($query);
    $stmt->execute();
    var_dump($stmt->fetch(PDO::FETCH_NUM) === [0 => 1]);
}

// One parameter
$queries = [
    "SELECT * FROM {$table} WHERE '1' = ?",
    "SELECT * FROM {$table} WHERE \"?\" IN (?, \"?\")",
    "SELECT * FROM {$table} WHERE `a``?` = ?",
    "SELECT * FROM {$table} WHERE \"a`?\" = ?",
    "SELECT * FROM {$table} WHERE [a`?] = ?",
];

foreach ($queries as $k => $query) {
    $stmt = $db->prepare($query);
    $stmt->execute([1]);
    var_dump($stmt->fetch(PDO::FETCH_NUM) === [0 => 1]);
}

?>
--CLEAN--
<?php
$filename = __DIR__ . DIRECTORY_SEPARATOR . "pdo_sqlite_parser.db";
if (file_exists($filename)) {
    unlink($filename);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
