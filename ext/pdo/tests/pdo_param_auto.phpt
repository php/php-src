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
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

$db->exec('DROP TABLE IF EXISTS test');

switch ($db->getAttribute(PDO::ATTR_DRIVER_NAME)) {
    case 'mysql':
        $bitType = 'bit';
        break;
    case 'pgsql':
    case 'firebird':
        $bitType = 'boolean';
        break;
    default:
        $bitType = 'int';
}

$db->exec(<<<SQL
CREATE TABLE test(
    f_int int NOT NULL,
    f_bool {$bitType} NOT NULL,
    f_null int NULL,
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
$stmt->bindValue(5, 'test', PDO::PARAM_AUTO);
if (!$stmt->execute()) {
    printf("Failed to INSERT data, %s\n", var_export($stmt->errorInfo(), true));
    exit(1);
}

fclose($fp);

$query_stmt = $db->query('SELECT * FROM test WHERE f_int = 123');
$row = $query_stmt->fetch(PDO::FETCH_ASSOC);
var_dump($row);
var_dump(1 === $row['f_bool'] || true === $row['f_bool']);
?>
--CLEAN--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec('DROP TABLE IF EXISTS test');
@unlink(PDOTest::get_temp_dir() . DIRECTORY_SEPARATOR . 'pdoblob.tst');
?>
--EXPECTF--
array(5) {
  ["f_int"]=>
  int(123)
  ["f_bool"]=>
  %s(%s)
  ["f_null"]=>
  NULL
  ["f_lob"]=>
  string(12) "blob content"
  ["f_str"]=>
  string(4) "test"
}
bool(true)
