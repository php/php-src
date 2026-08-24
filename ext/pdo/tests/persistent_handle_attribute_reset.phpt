--TEST--
Persistent connection reuse resets PDO attributes to their defaults
--EXTENSIONS--
pdo
pdo_sqlite
--FILE--
<?php
$dsn = 'sqlite::memory:';
$options = [PDO::ATTR_PERSISTENT => true];

$p1 = new PDO($dsn, null, null, $options);
$p1->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
$p1->setAttribute(PDO::ATTR_ORACLE_NULLS, PDO::NULL_EMPTY_STRING);
$p1->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_NUM);
$p1->setAttribute(PDO::ATTR_CASE, PDO::CASE_UPPER);

unset($p1);

$p2 = new PDO($dsn, null, null, $options);

var_dump($p2->getAttribute(PDO::ATTR_STRINGIFY_FETCHES));
var_dump($p2->getAttribute(PDO::ATTR_ORACLE_NULLS) === PDO::NULL_NATURAL);
var_dump($p2->getAttribute(PDO::ATTR_DEFAULT_FETCH_MODE) === PDO::FETCH_BOTH);
var_dump($p2->getAttribute(PDO::ATTR_CASE) === PDO::CASE_NATURAL);
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(true)
