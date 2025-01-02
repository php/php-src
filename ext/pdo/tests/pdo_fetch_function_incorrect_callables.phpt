--TEST--
PDO Common: PDO::FETCH_FUNC with invalid callables
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$dummy_query = get_dummy_sql_request();
$stmt = $db->prepare($dummy_query);
$stmt->execute();

class Bar {
    static private function privateStatic($x, $y) {
        return $x;
    }

    public function instanceMethod($x, $y) {
        return $x .'==='. $y;
    }
}

try {
    var_dump($stmt->fetchAll(PDO::FETCH_FUNC, 'nothing'));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

try {
    var_dump($stmt->fetchAll(PDO::FETCH_FUNC, ''));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

try {
    var_dump($stmt->fetchAll(PDO::FETCH_FUNC, NULL));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

try {
    var_dump($stmt->fetchAll(PDO::FETCH_FUNC, 1));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

try {
    var_dump($stmt->fetchAll(PDO::FETCH_FUNC, ['self', 'foo']));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

try {
    var_dump($stmt->fetchAll(PDO::FETCH_FUNC, ['bar', 'instanceMethod']));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

try {
    var_dump($stmt->fetchAll(PDO::FETCH_FUNC, ['bar', 'privateStatic']));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

try {
    var_dump($stmt->fetchAll(PDO::FETCH_FUNC, ['bar', 'nonexistent']));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
TypeError: function "nothing" not found or invalid function name
TypeError: function "" not found or invalid function name
TypeError: PDOStatement::fetchAll(): Argument #2 must be a callable, null given
TypeError: no array or string given
TypeError: cannot access "self" when no class scope is active
TypeError: non-static method Bar::instanceMethod() cannot be called statically
TypeError: cannot access private method Bar::privateStatic()
TypeError: class Bar does not have a method "nonexistent"
