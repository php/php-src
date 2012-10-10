--TEST--
sqlite-oo: call method with $this
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; 
?>
--FILE--
<?php
include "blankdb_oo.inc";

$db->query("CREATE TABLE strings(key VARCHAR(10), var VARCHAR(10))");
$db->query("INSERT INTO strings VALUES('foo', 'foo')");

class sqlite_help
{
	function __construct($db){
		$this->db = $db;
		$this->db->createFunction('link_keywords', array(&$this, 'linkers'), 1);
	}

	function getSingle($key)
	{
		return $this->db->singleQuery('SELECT link_keywords(var) FROM strings WHERE key=\''.$key.'\'', 1);
	}

	function linkers($str)
	{
		$str = str_replace('foo', 'bar', $str);
		return $str;
	}

	function free()
	{
		unset($this->db);
	}

	function __destruct()
	{
		echo "DESTRUCTED\n";
	}
}

$obj = new sqlite_help($db);
echo $obj->getSingle('foo')."\n";
$obj->free();
unset($obj);

?>
===DONE===
--EXPECT--
bar
===DONE===
DESTRUCTED
