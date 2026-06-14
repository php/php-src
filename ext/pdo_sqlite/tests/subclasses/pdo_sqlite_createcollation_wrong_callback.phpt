--TEST--
Pdo\Sqlite::createCollation() memory leaks on wrong callback return type
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

declare(strict_types=1);

$db = new Pdo\Sqlite('sqlite::memory:');

$db->exec("CREATE TABLE test (c string)");
$db->exec("INSERT INTO test VALUES('youwontseeme')");
$db->exec("INSERT INTO test VALUES('neither')");
$db->createCollation('NAT', function($a, $b): string { return $a . $b; });

try {
    $db->query("SELECT c FROM test ORDER BY c COLLATE NAT");
} catch (\TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
PDO::query(): Return value of the collation callback must be of type int, string returned
