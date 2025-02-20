--TEST--
PDO Common: PDO::FETCH_LAZY set via PDOStatement::setFetchMode()
--DESCRIPTION--
This test uses an EXPECT section because we want to know the object handlers
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

$db->exec('CREATE TABLE pdo_fetch_lazy_as_default_changed(id int NOT NULL PRIMARY KEY, val1 VARCHAR(10), val2 VARCHAR(10), grp VARCHAR(10))');
// Firebird does not allow inserting multiple rows with INSERT INTO
$db->exec("INSERT INTO pdo_fetch_lazy_as_default_changed VALUES (1, 'A', 'alpha', 'Group1')");
$db->exec("INSERT INTO pdo_fetch_lazy_as_default_changed VALUES (2, 'B', 'beta', 'Group1')");
$db->exec("INSERT INTO pdo_fetch_lazy_as_default_changed VALUES (3, 'C', 'gamma', 'Group2')");
$db->exec("INSERT INTO pdo_fetch_lazy_as_default_changed VALUES (4, 'D', 'delta', 'Group2')");

function to_upper_with_log(string $str): string {
    echo __FUNCTION__, '(', var_export($str, true), ')', PHP_EOL;
    return strtoupper($str);
}

$pdoQuery = $db->prepare('SELECT val2, val1 FROM pdo_fetch_lazy_as_default_changed');
$pdoQuery->execute();
$pdoQuery->setFetchMode(PDO::FETCH_LAZY);

class Test {
    public $val1;
    public $val2;
}
$o = new Test();

var_dump($pdoQuery->fetch());
var_dump($pdoQuery->fetchObject(Test::class));
var_dump($pdoQuery->fetch());
$pdoQuery->setFetchMode(PDO::FETCH_INTO, $o);
var_dump($pdoQuery->fetch());
var_dump($o);

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_lazy_as_default_changed");
?>
--EXPECT--
object(PDORow)#4 (3) {
  ["queryString"]=>
  string(56) "SELECT val2, val1 FROM pdo_fetch_lazy_as_default_changed"
  ["val2"]=>
  string(5) "alpha"
  ["val1"]=>
  string(1) "A"
}
object(Test)#4 (2) {
  ["val1"]=>
  string(1) "B"
  ["val2"]=>
  string(4) "beta"
}
object(PDORow)#4 (3) {
  ["queryString"]=>
  string(56) "SELECT val2, val1 FROM pdo_fetch_lazy_as_default_changed"
  ["val2"]=>
  string(5) "gamma"
  ["val1"]=>
  string(1) "C"
}
object(Test)#3 (2) {
  ["val1"]=>
  string(1) "D"
  ["val2"]=>
  string(5) "delta"
}
object(Test)#3 (2) {
  ["val1"]=>
  string(1) "D"
  ["val2"]=>
  string(5) "delta"
}
