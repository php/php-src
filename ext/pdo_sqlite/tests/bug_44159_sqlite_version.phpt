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

try {
    var_dump($pdo->setAttribute(PDO::NULL_TO_STRING, NULL));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
var_dump($pdo->setAttribute(PDO::NULL_TO_STRING, 1));
try {
    var_dump($pdo->setAttribute(PDO::NULL_TO_STRING, 'nonsense'));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

@unlink(__DIR__."/foo.db");

?>
--EXPECT--
Attribute value must be of type int for selected attribute, null given
bool(true)
Attribute value must be of type int for selected attribute, string given
