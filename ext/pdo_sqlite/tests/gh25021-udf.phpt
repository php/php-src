--TEST--
GH-25021 (UDF isn't using sqlite3_*_int64 API)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

$conn = new PDO('sqlite::memory:');

$conn->sqliteCreateFunction('my_udf_pass_int', static fn(int $v): int => $v);

foreach ([
    10,
    PHP_INT_MIN,
    PHP_INT_MAX,
] as $v) {
    $stmt = $conn->prepare('select my_udf_pass_int(:v)');
    $stmt->bindValue(':v', $v, PDO::PARAM_INT);
    $stmt->execute();
    $result = $stmt->fetch();
    
    var_dump($result[0] === $v);
}
?>
--EXPECTF--
Deprecated: Method PDO::sqliteCreateFunction() is deprecated since 8.5, use Pdo\Sqlite::createFunction() instead in %s on line %d
bool(true)
bool(true)
bool(true)
