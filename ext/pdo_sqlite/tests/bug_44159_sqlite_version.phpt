--TEST--
PDO Common: Bug #44159 (Crash: $pdo->setAttribute(PDO::STATEMENT_ATTR_CLASS, NULL)): SQLite variant
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip PDO not available');
try {
    $pdo = new PDO("sqlite:".__DIR__."/foo.db");
} catch (Exception $e) {
    die("skip PDP_SQLITE not available");
}
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
