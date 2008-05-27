--TEST--
PDO Common: serializing
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo')) die('skip');
if (!interface_exists('Serializable')) die('skip no Serializable interface');
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

class TestBase implements Serializable
{
	public    $BasePub = 'Public';
	protected $BasePro = 'Protected';
	private   $BasePri = 'Private';
	
	function serialize()
	{
		$serialized = array();
		foreach($this as $prop => $val) {
			$serialized[$prop] = $val;
		}
		$serialized = serialize($serialized);
		echo __METHOD__ . "() = '$serialized'\n";
		return $serialized;
	}
	
	function unserialize($serialized)
	{
		echo __METHOD__ . "($serialized)\n";
		foreach(unserialize($serialized) as $prop => $val) {
			$this->$prop = '#'.$val;
		}
		return true;
	}
}

class TestDerived extends TestBase
{
	public    $BasePub    = 'DerivedPublic';
	protected $BasePro    = 'DerivdeProtected';
	public    $DerivedPub = 'Public';
	protected $DerivedPro = 'Protected';
	private   $DerivedPri = 'Private';

	function serialize()
	{
		echo __METHOD__ . "()\n";
		return TestBase::serialize();
	}
	
	function unserialize($serialized)
	{
		echo __METHOD__ . "()\n";
		return TestBase::unserialize($serialized);
	}
}

class TestLeaf extends TestDerived
{
}

$db->exec('CREATE TABLE classtypes(id int NOT NULL PRIMARY KEY, name VARCHAR(20) NOT NULL UNIQUE)');
$db->exec('INSERT INTO classtypes VALUES(0, \'stdClass\')'); 
$db->exec('INSERT INTO classtypes VALUES(1, \'TestBase\')'); 
$db->exec('INSERT INTO classtypes VALUES(2, \'TestDerived\')'); 
$db->exec('CREATE TABLE test(id int NOT NULL PRIMARY KEY, classtype int, val VARCHAR(255))');

$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

var_dump($db->query('SELECT COUNT(*) FROM classtypes')->fetchColumn());
var_dump($db->query('SELECT id, name FROM classtypes ORDER by id')->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE));

$objs = array();
$objs[0] = new stdClass;
$objs[1] = new TestBase;
$objs[2] = new TestDerived;
$objs[3] = new TestLeaf;

$stmt = $db->prepare('SELECT id FROM classtypes WHERE name=:cname');
$stmt->bindParam(':cname', $cname);

$ctypes = array();

foreach($objs as $obj)
{
	$cname = get_class($obj);
	$ctype = NULL; /* set default for non stored class name */
	$stmt->execute();
	$stmt->bindColumn('id', $ctype);
	$stmt->fetch(PDO::FETCH_BOUND);
	$ctypes[$cname] = $ctype;
}

echo "===TYPES===\n";
var_dump($ctypes);

unset($stmt);

echo "===INSERT===\n";
$stmt = $db->prepare('INSERT INTO test VALUES(:id, :classtype, :val)');
$stmt->bindParam(':id', $idx);
$stmt->bindParam(':classtype', $ctype);
$stmt->bindParam(':val', $val);

foreach($objs as $idx => $obj)
{
	$ctype = $ctypes[get_class($obj)];
	if (method_exists($obj, 'serialize'))
	{
		$val = $obj->serialize();
	}
	else
	{
		$val = '';
	}
	$stmt->execute();	
}

unset($stmt);

echo "===DATA===\n";
var_dump($db->query('SELECT test.val FROM test')->fetchAll(PDO::FETCH_COLUMN));

echo "===FAILURE===\n";
try
{
	$db->query('SELECT classtypes.name AS name, test.val AS val FROM test LEFT JOIN classtypes ON test.classtype=classtypes.id')->fetchAll(PDO::FETCH_CLASS|PDO::FETCH_CLASSTYPE|PDO::FETCH_SERIALIZE, 'TestLeaf', array());
}
catch (PDOException $e)
{
	echo 'Exception:';
	echo $e->getMessage()."\n";
}

echo "===COUNT===\n";
var_dump($db->query('SELECT COUNT(*) FROM test LEFT JOIN classtypes ON test.classtype=classtypes.id WHERE (classtypes.id IS NULL OR classtypes.id > 0)')->fetchColumn());

echo "===DATABASE===\n";
$stmt = $db->prepare('SELECT classtypes.name AS name, test.val AS val FROM test LEFT JOIN classtypes ON test.classtype=classtypes.id WHERE (classtypes.id IS NULL OR classtypes.id > 0)');

$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

echo "===FETCHCLASS===\n";
$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_CLASS|PDO::FETCH_CLASSTYPE|PDO::FETCH_SERIALIZE, 'TestLeaf'));


?>
--EXPECTF--
unicode(1) "3"
array(3) {
  [0]=>
  unicode(8) "stdClass"
  [1]=>
  unicode(8) "TestBase"
  [2]=>
  unicode(11) "TestDerived"
}
===TYPES===
array(4) {
  [u"stdClass"]=>
  unicode(1) "0"
  [u"TestBase"]=>
  unicode(1) "1"
  [u"TestDerived"]=>
  unicode(1) "2"
  [u"TestLeaf"]=>
  NULL
}
===INSERT===
TestBase::serialize() = 'a:3:{s:7:"BasePub";s:6:"Public";s:7:"BasePro";s:9:"Protected";s:7:"BasePri";s:7:"Private";}'
TestDerived::serialize()
TestBase::serialize() = 'a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";}'
TestDerived::serialize()
TestBase::serialize() = 'a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";}'
===DATA===
array(4) {
  [0]=>
  unicode(0) ""
  [1]=>
  unicode(91) "a:3:{s:7:"BasePub";s:6:"Public";s:7:"BasePro";s:9:"Protected";s:7:"BasePri";s:7:"Private";}"
  [2]=>
  unicode(144) "a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";}"
  [3]=>
  unicode(144) "a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";}"
}
===FAILURE===
Exception:SQLSTATE[HY000]: General error: cannot unserialize class
===COUNT===
unicode(1) "3"
===DATABASE===
array(3) {
  [0]=>
  array(2) {
    [u"name"]=>
    unicode(8) "TestBase"
    [u"val"]=>
    unicode(91) "a:3:{s:7:"BasePub";s:6:"Public";s:7:"BasePro";s:9:"Protected";s:7:"BasePri";s:7:"Private";}"
  }
  [1]=>
  array(2) {
    [u"name"]=>
    unicode(11) "TestDerived"
    [u"val"]=>
    unicode(144) "a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";}"
  }
  [2]=>
  array(2) {
    [u"name"]=>
    NULL
    [u"val"]=>
    unicode(144) "a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";}"
  }
}
===FETCHCLASS===
TestBase::unserialize(a:3:{s:7:"BasePub";s:6:"Public";s:7:"BasePro";s:9:"Protected";s:7:"BasePri";s:7:"Private";})
TestDerived::unserialize()
TestBase::unserialize(a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";})
TestDerived::unserialize()
TestBase::unserialize(a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";})
array(3) {
  [0]=>
  object(TestBase)#%d (3) {
    [u"BasePub"]=>
    unicode(7) "#Public"
    [u"BasePro":protected]=>
    unicode(10) "#Protected"
    [u"BasePri":u"TestBase":private]=>
    unicode(8) "#Private"
  }
  [1]=>
  object(TestDerived)#%d (6) {
    [u"BasePub"]=>
    unicode(14) "#DerivedPublic"
    [u"BasePro":protected]=>
    unicode(17) "#DerivdeProtected"
    [u"DerivedPub"]=>
    unicode(7) "#Public"
    [u"DerivedPro":protected]=>
    unicode(10) "#Protected"
    [u"DerivedPri":u"TestDerived":private]=>
    unicode(7) "Private"
    [u"BasePri":u"TestBase":private]=>
    unicode(7) "Private"
  }
  [2]=>
  object(TestLeaf)#%d (6) {
    [u"BasePub"]=>
    unicode(14) "#DerivedPublic"
    [u"BasePro":protected]=>
    unicode(17) "#DerivdeProtected"
    [u"DerivedPub"]=>
    unicode(7) "#Public"
    [u"DerivedPro":protected]=>
    unicode(10) "#Protected"
    [u"DerivedPri":u"TestDerived":private]=>
    unicode(7) "Private"
    [u"BasePri":u"TestBase":private]=>
    unicode(7) "Private"
  }
}
