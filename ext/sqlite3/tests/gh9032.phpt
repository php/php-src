--TEST--
SQLite3 authorizer crashes on NULL values
--SKIPIF--
<?php
if (!extension_loaded("sqlite3")) die("skip sqlite3 extension not available");
?>
--INI--
open_basedir=.
--FILE--
<?php
$db = new SQLite3(":memory:");
$db->enableExceptions(true);

$db->exec('attach database \':memory:\' AS "db1"');
var_dump($db->exec('create table db1.r (id int)'));

try {
    $st = $db->prepare('attach database :a AS "db2"');
    $st->bindValue("a", ":memory:");
    $st->execute();
    var_dump($db->exec('create table db2.r (id int)'));
} catch (Exception $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
bool(true)
Unable to prepare statement: 23, not authorized
