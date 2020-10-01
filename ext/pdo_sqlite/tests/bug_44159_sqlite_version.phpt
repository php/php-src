--TEST--
PDO Common: Bug #44159: SQLite variant
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) die('skip PDO SQLite not available');
?>
--FILE--
<?php
$pdo = new PDO("sqlite:".__DIR__."/foo.db");
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

var_dump($pdo->setAttribute(PDO::NULL_TO_STRING, NULL));
var_dump($pdo->setAttribute(PDO::NULL_TO_STRING, 1));
var_dump($pdo->setAttribute(PDO::NULL_TO_STRING, 'nonsense'));

@unlink(__DIR__."/foo.db");

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
