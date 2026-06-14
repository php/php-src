--TEST--
PDO Common: PDO::FETCH_FUNC with a call that throws an exception for one specific value
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

$db->exec('CREATE TABLE pdo_fetch_function_exception_in_call(id int NOT NULL PRIMARY KEY, val1 VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_function_exception_in_call VALUES(1, 'A', 'alpha')");
$db->exec("INSERT INTO pdo_fetch_function_exception_in_call VALUES(2, 'B', 'beta')");
$db->exec("INSERT INTO pdo_fetch_function_exception_in_call VALUES(3, 'C', 'gamma')");
$db->exec("INSERT INTO pdo_fetch_function_exception_in_call VALUES(4, 'D', 'delta')");

$selectVals = $db->prepare('SELECT val1, val2 FROM pdo_fetch_function_exception_in_call');

function bogusCallback(string $val1, string $val2) {
    $r = $val1 . ': ' . $val2 . PHP_EOL;
    echo $r;
    if ($val2 === 'gamma') {
        throw new Exception("GAMMA IS BAD");
    }
    return $val1 . ': ' . $val2;
}

$selectVals->execute();

try {
    $result = $selectVals->fetchAll(PDO::FETCH_FUNC, 'bogusCallback');
    var_dump($result);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_function_exception_in_call");
?>
--EXPECT--
A: alpha
B: beta
C: gamma
Exception: GAMMA IS BAD
