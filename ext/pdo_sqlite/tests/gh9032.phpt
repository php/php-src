--TEST--
SQLite3 authorizer crashes on NULL values
--SKIPIF--
<?php
if (!extension_loaded("pdo_sqlite")) die("skip pdo_sqlite extension not available");
?>
--INI--
open_basedir=.
--FILE--
<?php
$db = new PDO("sqlite::memory:", null, null, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);

$db->exec('attach database \':memory:\' AS "db1"');
var_dump($db->exec('create table db1.r (id int)'));

try {
$st = $db->prepare('attach database :a AS "db2"');
$st->execute([':a' => ':memory:']);
var_dump($db->exec('create table db2.r (id int)'));
} catch (PDOException $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
int(0)
SQLSTATE[HY000]: General error: 23 not authorized
