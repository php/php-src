--TEST--
PDO_odbc getColumnMeta() bounds checking on unexecuted statements and invalid indexes
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
try {
    new Pdo\Odbc('odbc:Driver={SQLite3};Database=:memory:');
} catch (Throwable $e) {
    die('skip SQLite3 ODBC driver not available');
}
?>
--FILE--
<?php
$pdo = new Pdo\Odbc('odbc:Driver={SQLite3};Database=:memory:',
    null, null, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
$stmt = $pdo->prepare('SELECT 1 AS one');

try {
    var_dump($stmt->getColumnMeta(0));
} catch (PDOException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$stmt->execute();
var_dump($stmt->getColumnMeta(0));

try {
    var_dump($stmt->getColumnMeta(5));
} catch (PDOException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "done\n";
?>
--EXPECTF--
PDOException: SQLSTATE[07009]: Invalid descriptor index: invalid column index
array(4) {
  ["pdo_type"]=>
  int(2)
  ["name"]=>
  string(3) "one"
  ["len"]=>
  int(%d)
  ["precision"]=>
  int(%d)
}
PDOException: SQLSTATE[07009]: Invalid descriptor index: invalid column index
done
