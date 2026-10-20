--TEST--
PDO Common: PDO::FETCH_FUNC and statement overloading
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

$db->exec('CREATE TABLE pdo_fetch_function_overload_statement_class(id int NOT NULL PRIMARY KEY, val VARCHAR(10))');
$db->exec("INSERT INTO pdo_fetch_function_overload_statement_class VALUES (1, 'A')");
$db->exec("INSERT INTO pdo_fetch_function_overload_statement_class VALUES (2, 'B')");
$db->exec("INSERT INTO pdo_fetch_function_overload_statement_class VALUES (3, 'C')");
$db->exec("INSERT INTO pdo_fetch_function_overload_statement_class VALUES (4, 'D')");

class DerivedStatement extends PDOStatement
{
    private function __construct(public $name, $db)
    {
        echo __METHOD__ . "($name)\n";
    }

    function reTrieve($id, $val) {
        echo __METHOD__ . "($id,$val)\n";
        return "$id => $val";
    }
}

$derived = $db->prepare('SELECT id, val FROM pdo_fetch_function_overload_statement_class', [PDO::ATTR_STATEMENT_CLASS => ['DerivedStatement', ['Overloaded', $db]]]);
var_dump(get_class($derived));
$derived->execute();
var_dump($derived->fetchAll(PDO::FETCH_FUNC, [$derived, 'retrieve']));

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "pdo_fetch_function_overload_statement_class");
?>
--EXPECT--
DerivedStatement::__construct(Overloaded)
string(16) "DerivedStatement"
DerivedStatement::reTrieve(1,A)
DerivedStatement::reTrieve(2,B)
DerivedStatement::reTrieve(3,C)
DerivedStatement::reTrieve(4,D)
array(4) {
  [0]=>
  string(6) "1 => A"
  [1]=>
  string(6) "2 => B"
  [2]=>
  string(6) "3 => C"
  [3]=>
  string(6) "4 => D"
}
