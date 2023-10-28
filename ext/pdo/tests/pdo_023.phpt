--TEST--
PDO Common: extending PDO
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

#[AllowDynamicProperties]
class PDOStatementX extends PDOStatement
{
    public $test1 = 1;

    protected function __construct()
    {
        $this->test2 = 2;
        $this->test2 = 22;
        echo __METHOD__ . "()\n";
    }

    function __destruct()
    {
        echo __METHOD__ . "()\n";
    }
}

#[AllowDynamicProperties]
class PDODatabaseX extends PDO
{
    public $test1 = 1;

    function __destruct()
    {
        echo __METHOD__ . "()\n";
    }

    function test()
    {
        $this->test2 = 2;
        var_dump($this->test1);
        var_dump($this->test2);
        $this->test2 = 22;
    }

    function query($sql, ...$rest): PDOStatement|false
    {
        echo __METHOD__ . "()\n";
        $stmt = parent::prepare($sql, array(PDO::ATTR_STATEMENT_CLASS=>array('PDOStatementx')));
        $stmt->execute();
        return $stmt;
    }
}

$db = PDOTest::factory('PDODatabaseX');
$db->test();
var_dump($db);

$db->query('CREATE TABLE test023(id INT NOT NULL PRIMARY KEY, val VARCHAR(10))');
$db->query("INSERT INTO test023 VALUES(0, 'A')");
$db->query("INSERT INTO test023 VALUES(1, 'B')");


$stmt = $db->query('SELECT val, id FROM test023');
var_dump($stmt);
var_dump($stmt->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE));

$stmt = NULL;
$db = NULL;


?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test023");
?>
--EXPECTF--
int(1)
int(2)
object(PDODatabaseX)#%d (2) {
  ["test1"]=>
  int(1)
  ["test2"]=>
  int(22)
}
PDODatabaseX::query()
PDOStatementX::__construct()
PDOStatementX::__destruct()
PDODatabaseX::query()
PDOStatementX::__construct()
PDOStatementX::__destruct()
PDODatabaseX::query()
PDOStatementX::__construct()
PDOStatementX::__destruct()
PDODatabaseX::query()
PDOStatementX::__construct()
object(PDOStatementX)#%d (3) {
  ["queryString"]=>
  string(27) "SELECT val, id FROM test023"
  ["test1"]=>
  int(1)
  ["test2"]=>
  int(22)
}
array(2) {
  ["A"]=>
  string(1) "0"
  ["B"]=>
  string(1) "1"
}
PDOStatementX::__destruct()
PDODatabaseX::__destruct()
