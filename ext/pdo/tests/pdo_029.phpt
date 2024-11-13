--TEST--
PDO Common: extending PDO (3)
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

$data = array(
    array('10', 'Abc', 'zxy'),
    array('20', 'Def', 'wvu'),
    array('30', 'Ghi', 'tsr'),
);

class PDOStatementX extends PDOStatement
{
    public $dbh;

    protected function __construct($dbh)
    {
        $this->dbh = $dbh;
        echo __METHOD__ . "()\n";
    }

    function __destruct()
    {
        echo __METHOD__ . "()\n";
    }

    function execute($params = array()): bool
    {
        echo __METHOD__ . "()\n";
        return parent::execute();
    }
}

class PDODatabase extends PDO
{
    function __destruct()
    {
        echo __METHOD__ . "()\n";
    }

    function query($sql, ...$rest): PDOStatement|false
    {
        echo __METHOD__ . "()\n";
        $stmt = $this->prepare($sql, array(PDO::ATTR_STATEMENT_CLASS=>array('PDOStatementx', array($this))));
        $stmt->setFetchMode(PDO::FETCH_ASSOC);
        $stmt->execute();
        return $stmt;
    }
}

$db = PDOTest::factory('PDODatabase');
var_dump(get_class($db));

$db->exec('CREATE TABLE test029(id INT NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(16))');

$stmt = $db->prepare("INSERT INTO test029 VALUES(?, ?, ?)");
var_dump(get_class($stmt));
foreach ($data as $row) {
    $stmt->execute($row);
}

unset($stmt);

echo "===QUERY===\n";

$stmt = $db->query('SELECT * FROM test029');
var_dump(get_class($stmt));
var_dump(get_class($stmt->dbh));

echo "===FOREACH===\n";

foreach($stmt as $obj) {
    var_dump($obj);
}

echo "===DONE===\n";
exit(0);
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test029");
?>
--EXPECT--
string(11) "PDODatabase"
string(12) "PDOStatement"
===QUERY===
PDODatabase::query()
PDOStatementX::__construct()
PDOStatementX::execute()
string(13) "PDOStatementX"
string(11) "PDODatabase"
===FOREACH===
array(3) {
  ["id"]=>
  string(2) "10"
  ["val"]=>
  string(3) "Abc"
  ["val2"]=>
  string(3) "zxy"
}
array(3) {
  ["id"]=>
  string(2) "20"
  ["val"]=>
  string(3) "Def"
  ["val2"]=>
  string(3) "wvu"
}
array(3) {
  ["id"]=>
  string(2) "30"
  ["val"]=>
  string(3) "Ghi"
  ["val2"]=>
  string(3) "tsr"
}
===DONE===
PDOStatementX::__destruct()
PDODatabase::__destruct()
