--TEST--
PDO Common: PDO::PARAM_AUTO
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();

$tmp = PDOTest::get_temp_dir();
if (!$tmp)
    die("skip Can't create temporary file");

$file = $tmp . DIRECTORY_SEPARATOR . 'pdoblob.tst';
$fp = fopen($file, 'w');
if (!$fp)
    die("skip Can't create temporary file");

if (4 != fwrite($fp, 'test')) {
    die("skip Can't create temporary file");
}
fclose($fp);
clearstatcache();

if (!file_exists($file))
    die("skip Can't create temporary file");

unlink($file);
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$null = 'NULL';
switch ($db->getAttribute(PDO::ATTR_DRIVER_NAME)) {
    case 'mysql':
        $bitType = 'bit';
        break;
    case 'pgsql':
        $bitType = 'boolean';
        break;
    case 'firebird':
        $bitType = 'boolean';
        $null = '';
        break;
    default:
        $bitType = 'int';
}

$db->exec(<<<SQL
CREATE TABLE test(
    f_int int NOT NULL,
    f_bool {$bitType} NOT NULL,
    f_null int {$null},
    f_lob varchar(32) NOT NULL,
    f_str varchar(32) NOT NULL
)
SQL);

$file = PDOTest::get_temp_dir() . DIRECTORY_SEPARATOR . 'pdoblob.tst';
file_put_contents($file, 'blob content');
$fp = fopen($file, 'r');
if (!$fp) {
    printf("Cannot create test file '%s'\n", $file);
    exit(1);
}

$stmt = $db->prepare('INSERT INTO test VALUES(?,?,?,?,?)');
$stmt->bindValue(1, 123, PDO::PARAM_AUTO);
$stmt->bindValue(2, true, PDO::PARAM_AUTO);
$stmt->bindValue(3, null, PDO::PARAM_AUTO);
$stmt->bindValue(4, $fp, PDO::PARAM_AUTO);
$stmt->bindValue(5, 'test1', PDO::PARAM_AUTO);
if (!$stmt->execute()) {
    printf("Failed to INSERT data, %s\n", var_export($stmt->errorInfo(), true));
    exit(1);
}
$query_stmt = $db->query('SELECT * FROM test WHERE f_int = 123');
$row = $query_stmt->fetch(PDO::FETCH_ASSOC);
var_dump(123 == $row['f_int']);
var_dump(1 == $row['f_bool']);
var_dump(null === $row['f_null']);
var_dump('blob content' === $row['f_lob']);
var_dump('test1' === $row['f_str']);

fseek($fp, 0);
$stmt->bindValue(1, 456, PDO::PARAM_AUTO);
$stmt->bindValue(2, false, PDO::PARAM_AUTO);
$stmt->bindValue(3, 789, PDO::PARAM_AUTO);
$stmt->bindValue(4, $fp, PDO::PARAM_AUTO);
$stmt->bindValue(5, 'test2', PDO::PARAM_AUTO);
if (!$stmt->execute()) {
    printf("Failed to INSERT data, %s\n", var_export($stmt->errorInfo(), true));
    exit(1);
}
$query_stmt = $db->query('SELECT * FROM test WHERE f_int = 456');
$row = $query_stmt->fetch(PDO::FETCH_ASSOC);
var_dump(456 == $row['f_int']);
var_dump(0 == $row['f_bool']);
var_dump(789 == $row['f_null']);
var_dump('blob content' === $row['f_lob']);
var_dump('test2' === $row['f_str']);

fclose($fp);
?>
--CLEAN--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test');
@unlink(PDOTest::get_temp_dir() . DIRECTORY_SEPARATOR . 'pdoblob.tst');
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
