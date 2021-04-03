--TEST--
Bug #73333 (2147483647 is fetched as string)
--EXTENSIONS--
sqlite3
--FILE--
<?php
if (!defined('PHP_INT_MIN')) define('PHP_INT_MIN', -PHP_INT_MAX-1);

$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE foo (bar INT)');
foreach ([PHP_INT_MIN, PHP_INT_MAX] as $value) {
    $db->exec("INSERT INTO foo VALUES ($value)");
}

$res = $db->query('SELECT bar FROM foo');
while (($row = $res->fetchArray(SQLITE3_NUM)) !== false) {
    echo gettype($row[0]), PHP_EOL;
}
?>
--EXPECT--
integer
integer
