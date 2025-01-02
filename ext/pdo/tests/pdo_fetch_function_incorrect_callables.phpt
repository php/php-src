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

$db->exec('CREATE TABLE pdo_fetch_function_incorrect_callable(id int NOT NULL PRIMARY KEY, pl_name VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_function_incorrect_callable VALUES (1, 'php')");
$db->exec("INSERT INTO pdo_fetch_function_incorrect_callable VALUES (2, 'sql')");
$stmt = $db->query('SELECT * FROM pdo_fetch_function_incorrect_callable');

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
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_function_incorrect_callable");
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
