--TEST--
PDO Common: PDO::FETCH_INTO
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
$db = PDOTest::factory();

$db->exec('CREATE TABLE test(id INT NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(16))');

$data = array(
    array('10', 'Abc', 'zxy'),
    array('20', 'Def', 'wvu'),
    array('30', 'Ghi', 'tsr'),
    array('40', 'Jkl', 'qpo'),
    array('50', 'Mno', 'nml'),
    array('60', 'Pqr', 'kji'),
);


// Insert using question mark placeholders
$stmt = $db->prepare("INSERT INTO test VALUES(?, ?, ?)");
foreach ($data as $row) {
    $stmt->execute($row);
}

class Test {
	public $id, $val, $val2;
}

$stmt = $db->prepare('SELECT * FROM test');
$stmt->setFetchMode(PDO::FETCH_INTO, new Test);
$stmt->execute();

foreach($stmt as $obj) {
	var_dump($obj);
}

echo "===FAIL===\n";

class Fail {
	protected $id;
	public $val, $val2;
}

$stmt->setFetchMode(PDO::FETCH_INTO, new Fail);
$stmt->execute();

foreach($stmt as $obj) {
	var_dump($obj);
}

?>
--EXPECTF--
object(Test)#%d (3) {
  [u"id"]=>
  unicode(2) "10"
  [u"val"]=>
  unicode(3) "Abc"
  [u"val2"]=>
  unicode(3) "zxy"
}
object(Test)#%d (3) {
  [u"id"]=>
  unicode(2) "20"
  [u"val"]=>
  unicode(3) "Def"
  [u"val2"]=>
  unicode(3) "wvu"
}
object(Test)#%d (3) {
  [u"id"]=>
  unicode(2) "30"
  [u"val"]=>
  unicode(3) "Ghi"
  [u"val2"]=>
  unicode(3) "tsr"
}
object(Test)#%d (3) {
  [u"id"]=>
  unicode(2) "40"
  [u"val"]=>
  unicode(3) "Jkl"
  [u"val2"]=>
  unicode(3) "qpo"
}
object(Test)#%d (3) {
  [u"id"]=>
  unicode(2) "50"
  [u"val"]=>
  unicode(3) "Mno"
  [u"val2"]=>
  unicode(3) "nml"
}
object(Test)#%d (3) {
  [u"id"]=>
  unicode(2) "60"
  [u"val"]=>
  unicode(3) "Pqr"
  [u"val2"]=>
  unicode(3) "kji"
}
===FAIL===

Fatal error: Cannot access protected property Fail::$id in %spdo_025.php on line %d
