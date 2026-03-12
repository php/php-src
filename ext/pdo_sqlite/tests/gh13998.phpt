--TEST--
Fix GH-13998: Manage refcount of agg_context->val correctly
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$step = function () {
    throw new Exception();
};
$finalize = function () {
};

$db = new PDO('sqlite::memory:');
$db->query('CREATE TABLE test (a int, b int)');
$stmt = $db->query('INSERT INTO test VALUES (1, 1), (2, 2), (3, 3)');
$db->sqliteCreateAggregate('S', $step, $finalize, 1);

try {
    $db->query('SELECT S(a) FROM test');
} catch (Exception $e) {
    echo 'done!';
}
?>
--EXPECTF--
Deprecated: Method PDO::sqliteCreateAggregate() is deprecated since 8.5, use Pdo\Sqlite::createAggregate() instead in %s on line %d
done!
