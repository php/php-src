--TEST--
GH-20214 (PDO::FETCH_DEFAULT unexpected behavior with PDOStatement::setFetchMode)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$db = new PDO("sqlite::memory:");
$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_OBJ);

// setFetchMode with FETCH_DEFAULT should use connection default (FETCH_OBJ)
$stmt = $db->query("SELECT 'v1' AS c1, 'v2' AS c2");
$stmt->setFetchMode(PDO::FETCH_DEFAULT);
$row = $stmt->fetch();
var_dump($row instanceof stdClass);
var_dump($row->c1);

// fetch with FETCH_DEFAULT should also use connection default
$stmt = $db->query("SELECT 'v1' AS c1, 'v2' AS c2");
$row = $stmt->fetch(PDO::FETCH_DEFAULT);
var_dump($row instanceof stdClass);

// fetchAll with FETCH_DEFAULT should also use connection default
$stmt = $db->query("SELECT 'v1' AS c1, 'v2' AS c2");
$rows = $stmt->fetchAll(PDO::FETCH_DEFAULT);
var_dump($rows[0] instanceof stdClass);

// setFetchMode then fetch without argument
$stmt = $db->query("SELECT 'v1' AS c1, 'v2' AS c2");
$stmt->setFetchMode(PDO::FETCH_DEFAULT);
$row = $stmt->fetch();
var_dump($row instanceof stdClass);
?>
--EXPECT--
bool(true)
string(2) "v1"
bool(true)
bool(true)
bool(true)
