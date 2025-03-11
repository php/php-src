--TEST--
PDO Common: PDO::FETCH_LAZY when FETCH_INTO set via PDOStatement::setFetchMode()
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

class Test {
    public $val1;
    public $val2;
}
$o = new Test();

$pdoQuery->setFetchMode(PDO::FETCH_INTO, $o);

var_dump($pdoQuery->fetch());
var_dump($pdoQuery->fetch(PDO::FETCH_LAZY));
var_dump($pdoQuery->fetch());

$another_obj = new stdClass();
var_dump($another_obj);

var_dump($pdoQuery->fetch(PDO::FETCH_LAZY));

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_lazy_as_default_changed");
?>
--EXPECT--
object(Test)#3 (2) {
  ["val1"]=>
  string(1) "A"
  ["val2"]=>
  string(5) "alpha"
}
object(PDORow)#4 (3) {
  ["queryString"]=>
  string(56) "SELECT val2, val1 FROM pdo_fetch_lazy_as_default_changed"
  ["val2"]=>
  string(4) "beta"
  ["val1"]=>
  string(1) "B"
}
object(Test)#3 (2) {
  ["val1"]=>
  string(1) "C"
  ["val2"]=>
  string(5) "gamma"
}
object(stdClass)#4 (0) {
}
object(PDORow)#5 (3) {
  ["queryString"]=>
  string(56) "SELECT val2, val1 FROM pdo_fetch_lazy_as_default_changed"
  ["val2"]=>
  string(5) "delta"
  ["val1"]=>
  string(1) "D"
}
