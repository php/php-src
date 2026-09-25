--TEST--
Pdo\Sqlite::ATTR_SQL and Pdo\Sqlite::ATTR_EXPANDED_SQL usage
--EXTENSIONS--
pdo_sqlite
--SKIPIF--
<?php
if (!defined('Pdo\Sqlite::ATTR_EXPANDED_SQL')) die('skip sqlite3_expanded_sql not available');
?>
--FILE--
<?php

$db = new Pdo\Sqlite('sqlite::memory:');

$stmt = $db->prepare('SELECT :name AS greeting, :num AS number');
var_dump($stmt->getAttribute(Pdo\Sqlite::ATTR_SQL));
var_dump($stmt->getAttribute(Pdo\Sqlite::ATTR_EXPANDED_SQL));

$stmt->bindValue(':name', 'hello world');
$stmt->bindValue(':num', 42, PDO::PARAM_INT);
$stmt->execute();

var_dump($stmt->getAttribute(Pdo\Sqlite::ATTR_SQL));
var_dump($stmt->getAttribute(Pdo\Sqlite::ATTR_EXPANDED_SQL));
?>
--EXPECT--
string(40) "SELECT :name AS greeting, :num AS number"
string(39) "SELECT NULL AS greeting, NULL AS number"
string(40) "SELECT :name AS greeting, :num AS number"
string(46) "SELECT 'hello world' AS greeting, 42 AS number"
