--TEST--
PDO Common: PDO::FETCH_CLASS with a constructor that changes the ctor args within PDO::fetchAll() (via warning and error handler)
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

// Warning to hook into error handler
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

class B {
    public function __construct() {}
}

$db->exec('CREATE TABLE pdo_fetch_class_change_ctor_five(id int NOT NULL PRIMARY KEY, val1 VARCHAR(10), val2 VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_class_change_ctor_five VALUES(1, 'A', 'alpha')");
$db->exec("INSERT INTO pdo_fetch_class_change_ctor_five VALUES(2, 'B', 'beta')");
$db->exec("INSERT INTO pdo_fetch_class_change_ctor_five VALUES(3, 'C', 'gamma')");
$db->exec("INSERT INTO pdo_fetch_class_change_ctor_five VALUES(4, 'D', 'delta')");

$stmt = $db->prepare('SELECT val1, val2 FROM pdo_fetch_class_change_ctor_five');
$stmt->execute();

function stuffingErrorHandler(int $errno, string $errstr, string $errfile, int $errline) {
    global $stmt;
    $stmt->setFetchMode(PDO::FETCH_CLASS, 'B', [$errstr]);
    echo $errstr, PHP_EOL;
}
set_error_handler(stuffingErrorHandler(...));

try {
    var_dump($stmt->fetchAll(PDO::FETCH_CLASS|PDO::FETCH_SERIALIZE, 'B', [$stmt]));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_class_change_ctor_five");
?>
--EXPECT--
PDOStatement::fetchAll(): The PDO::FETCH_SERIALIZE mode is deprecated
Error: Cannot change default fetch mode while fetching
