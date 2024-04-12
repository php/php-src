--TEST--
Bug #GH-13952 (sqlite PDO::quote silently corrupts strings with null bytes)
--EXTENSIONS--
pdo
pdo_sqlite
--FILE--
<?php
$pdo = new PDO("sqlite::memory:", null, null, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
var_dump($pdo->query("SELECT " . $pdo->quote("foo\x00bar"))->fetch(PDO::FETCH_NUM)[0] === "foo\x00bar");
?>
--EXPECT--
bool(true)
