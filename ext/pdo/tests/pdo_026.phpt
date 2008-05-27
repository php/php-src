--TEST--
PDO Common: extending PDO (2)
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
    	echo __METHOD__ . "()\n";
    }
    
    function __destruct()
    {
    	echo __METHOD__ . "()\n";
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
    	$stmt = $this->prepare($sql, array(PDO::ATTR_STATEMENT_CLASS=>array('PDOStatementx', array($this))));
    	$stmt->setFetchMode(PDO::FETCH_ASSOC);
    	$stmt->execute();
    	return $stmt;
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

$stmt = $db->query('SELECT * FROM test');
var_dump(get_class($stmt));
var_dump(get_class($stmt->dbh));

foreach($stmt as $obj) {
	var_dump($obj);
}

echo "===DONE===\n";
?>
--EXPECT--
unicode(11) "PDODatabase"
unicode(12) "PDOStatement"
PDODatabase::query()
PDOStatementX::__construct()
unicode(13) "PDOStatementX"
unicode(11) "PDODatabase"
array(3) {
  [u"id"]=>
  unicode(2) "10"
  [u"val"]=>
  unicode(3) "Abc"
  [u"val2"]=>
  unicode(3) "zxy"
}
array(3) {
  [u"id"]=>
  unicode(2) "20"
  [u"val"]=>
  unicode(3) "Def"
  [u"val2"]=>
  unicode(3) "wvu"
}
array(3) {
  [u"id"]=>
  unicode(2) "30"
  [u"val"]=>
  unicode(3) "Ghi"
  [u"val2"]=>
  unicode(3) "tsr"
}
===DONE===
PDOStatementX::__destruct()
PDODatabase::__destruct()
