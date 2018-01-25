--TEST--
PDO Common: extending PDO (4)
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.dirname(__FILE__) . '/../../pdo/tests/');
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
    	$this->setFetchMode(PDO::FETCH_ASSOC);
    	echo __METHOD__ . "()\n";
    }

    function __destruct()
    {
    	echo __METHOD__ . "()\n";
    }

    function execute($params = array())
    {
    	echo __METHOD__ . "()\n";
		parent::execute();
    }
}

class PDODatabase extends PDO
{
    function __destruct()
    {
    	echo __METHOD__ . "()\n";
    }

    function query($sql)
    {
    	echo __METHOD__ . "()\n";
    	return parent::query($sql);
    }
}

$db = PDOTest::factory('PDODatabase');
var_dump(get_class($db));

$db->exec('CREATE TABLE test(id INT NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(16))');

$stmt = $db->prepare("INSERT INTO test VALUES(?, ?, ?)");
var_dump(get_class($stmt));
foreach ($data as $row) {
    $stmt->execute($row);
}

unset($stmt);

echo "===QUERY===\n";

var_dump($db->getAttribute(PDO::ATTR_STATEMENT_CLASS));
$db->setAttribute(PDO::ATTR_STATEMENT_CLASS, array('PDOStatementx', array($db)));
var_dump($db->getAttribute(PDO::ATTR_STATEMENT_CLASS));
$stmt = $db->query('SELECT * FROM test');
var_dump(get_class($stmt));
var_dump(get_class($stmt->dbh));

echo "===FOREACH===\n";

foreach($stmt as $obj) {
	var_dump($obj);
}

echo "===DONE===\n";
exit(0);
?>
--EXPECTF--
string(11) "PDODatabase"
string(12) "PDOStatement"
===QUERY===
array(1) {
  [0]=>
  string(12) "PDOStatement"
}
array(2) {
  [0]=>
  string(13) "PDOStatementX"
  [1]=>
  array(1) {
    [0]=>
    object(PDODatabase)#%d (0) {
    }
  }
}
PDODatabase::query()
PDOStatementX::__construct()
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
