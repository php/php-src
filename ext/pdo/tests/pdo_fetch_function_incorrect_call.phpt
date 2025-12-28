--TEST--
PDO Common: PDO::FETCH_FUNC with a call that is invalid
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

$db->exec('CREATE TABLE pdo_fetch_function_incorrect_call(id int NOT NULL PRIMARY KEY, val1 VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_function_incorrect_call VALUES(1, 'A', 'alpha')");
$db->exec("INSERT INTO pdo_fetch_function_incorrect_call VALUES(2, 'B', 'beta')");
$db->exec("INSERT INTO pdo_fetch_function_incorrect_call VALUES(3, 'C', 'gamma')");
$db->exec("INSERT INTO pdo_fetch_function_incorrect_call VALUES(4, 'D', 'delta')");

$selectIdVal = $db->prepare('SELECT val1, val2 FROM pdo_fetch_function_incorrect_call');

function bogusCallback(stdClass $obj, string $str, array $arr) {
    echo "Called\n";
    return "how?";
}

$selectIdVal->execute();

echo "Fetch all with bogus call:\n";
try {
    $result = $selectIdVal->fetchAll(PDO::FETCH_FUNC, 'bogusCallback');
    var_dump($result);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_function_incorrect_call");
?>
--EXPECT--
Fetch all with bogus call:
TypeError: bogusCallback(): Argument #1 ($obj) must be of type stdClass, string given
