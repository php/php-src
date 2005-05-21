<?php # vim:ft=php

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
		echo __METHOD__ . '[' . __LINE__ . ']' . "($serialized)\n";
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

require_once('pdo.inc');
	
set_sql('create1', 'CREATE TABLE classtypes(id int NOT NULL PRIMARY KEY, name VARCHAR(20) UNIQUE)');
set_sql('insert1', 'INSERT INTO classtypes VALUES(0, \'stdClass\')'); 
set_sql('insert2', 'INSERT INTO classtypes VALUES(1, \'TestBase\')'); 
set_sql('insert3', 'INSERT INTO classtypes VALUES(2, \'TestDerived\')'); 
set_sql('selectC', 'SELECT COUNT(*) FROM classtypes'); 
set_sql('select0', 'SELECT id, name FROM classtypes ORDER by id'); 
set_sql('create2', 'CREATE TABLE test(id int NOT NULL PRIMARY KEY, classtype int, val VARCHAR(255))');
set_sql('insert4', 'INSERT INTO test VALUES(:id, :classtype, :val)');
set_sql('select1', 'SELECT id FROM classtypes WHERE name=:cname');
set_sql('select2', 'SELECT test.val FROM test');
set_sql('select3', 'SELECT classtypes.name AS name, test.val AS val FROM test LEFT JOIN classtypes ON test.classtype=classtypes.id');
set_sql('select4', 'SELECT COUNT(*) FROM test LEFT JOIN classtypes ON test.classtype=classtypes.id WHERE (classtypes.id IS NULL OR classtypes.id > 0)');
set_sql('select5', 'SELECT classtypes.name AS name, test.val AS val FROM test LEFT JOIN classtypes ON test.classtype=classtypes.id WHERE (classtypes.id IS NULL OR classtypes.id > 0)');

$DB->setAttribute(PDO_ATTR_ERRMODE, PDO_ERRMODE_EXCEPTION);

echo "===INIT===\n";
$DB->exec($SQL['create1']);
var_dump($DB->exec($SQL['insert1']));
var_dump($DB->exec($SQL['insert2']));
var_dump($DB->exec($SQL['insert3']));
$DB->exec($SQL['create2']);
var_dump($DB->query($SQL['selectC'])->fetchColumn());
var_dump($DB->query($SQL['select0'])->fetchAll(PDO_FETCH_COLUMN|PDO_FETCH_UNIQUE));

$objs = array();
$objs[0] = new stdClass;
$objs[1] = new TestBase;
$objs[2] = new TestDerived;
$objs[3] = new TestLeaf;

$stmt = $DB->prepare($SQL['select1']);
$stmt->bindParam(':cname', $cname);
$stmt->bindColumn('id', $ctype);

$ctypes = array();

foreach($objs as $obj)
{
	$cname = get_class($obj);
	$ctype = NULL; /* set default for non stored class name */
	$stmt->execute();
	$stmt->fetch(PDO_FETCH_BOUND);
	$ctypes[$cname] = $ctype;
}

echo "===TYPES===\n";
var_dump($ctypes);

echo "===INSERT===\n";
$stmt = $DB->prepare($SQL['insert4']);
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
		$val = NULL;
	}
	$stmt->execute();	
}

echo "===DATA===\n";
var_dump($DB->query($SQL['select2'])->fetchAll(PDO_FETCH_COLUMN));

echo "===FAILURE===\n";
try
{
	$DB->query($SQL['select3'])->fetchAll(PDO_FETCH_CLASS|PDO_FETCH_CLASSTYPE|PDO_FETCH_SERIALIZE, 'TestLeaf', array());
}
catch (PDOException $e)
{
	echo 'Exception:';
	echo $e->getMessage()."\n";
}

echo "===COUNT===\n";
var_dump($DB->query($SQL['select4'])->fetchColumn());

echo "===DATABASE===\n";
var_dump($DB->query($SQL['select5'])->fetchAll(PDO_FETCH_ASSOC));

echo "===FETCHCLASS===\n";
var_dump($DB->query($SQL['select5'])->fetchAll(PDO_FETCH_CLASS|PDO_FETCH_CLASSTYPE|PDO_FETCH_SERIALIZE, 'TestLeaf'));

?>
