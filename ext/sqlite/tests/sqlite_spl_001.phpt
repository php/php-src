--TEST--
sqlite-spl: Iteration
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; 
if (!extension_loaded("spl")) print "skip SPL is not present"; 
?>
--FILE--
<?php
include "blankdb_oo.inc";

$db->query("CREATE TABLE menu(id_l int PRIMARY KEY, id_r int UNIQUE, key VARCHAR(10))");
$db->query("INSERT INTO menu VALUES( 1, 12, 'A')"); 
$db->query("INSERT INTO menu VALUES( 2,  9, 'B')"); 
$db->query("INSERT INTO menu VALUES(10, 11, 'F')"); 
$db->query("INSERT INTO menu VALUES( 3,  6, 'C')"); 
$db->query("INSERT INTO menu VALUES( 7,  8, 'E')"); 
$db->query("INSERT INTO menu VALUES( 4,  5, 'D')"); 

class SqliteNestedsetElement
{
	protected $id_l;
	protected $id_r;
	protected $key;

	function __construct($db)
	{
		$this->db = $db;
	}
	
	function getLeft()
	{
		return $this->id_l;
	}
	
	function getRight()
	{
		return $this->id_r;
	}
	
	function __toString()
	{
		return $this->key;
	}

	function key()
	{
		return $this->key;
	}
}

class SqliteNestedset implements RecursiveIterator
{
	protected $id;
	protected $id_l;
	protected $id_r;
	protected $entry;

	function __construct($db, $id_l = 1)
	{
		$this->db = $db;
		$this->id_l = $id_l;
		$this->id_r = $this->db->singleQuery('SELECT id_r FROM menu WHERE id_l='.$id_l, 1);
		$this->id = $id_l;
	}
	
	function rewind()
	{
		$this->id = $this->id_l;
		$this->fetch();
	}

	function valid()
	{
		return is_object($this->entry);
	}
	
	function current()
	{
		return $this->entry->__toString();
	}
	
	function key()
	{
		return $this->entry->key();;
	}
	
	function next()
	{
		$this->id = $this->entry->getRight() + 1;
		$this->fetch();
	}

	protected function fetch()
	{
		$res = $this->db->unbufferedQuery('SELECT * FROM menu WHERE id_l='.$this->id);
		$this->entry = $res->fetchObject('SqliteNestedsetElement', array(&$this->db));
		unset($res);
	}
	
	function hasChildren()
	{
		return $this->entry->getLeft() + 1 < $this->entry->getRight();
	}
	
	function getChildren()
	{
		return new SqliteNestedset($this->db, $this->entry->getLeft() + 1, $this->entry->getRight() - 1);
	}
}

$menu_iterator = new RecursiveIteratorIterator(new SqliteNestedset($db), RecursiveIteratorIterator::SELF_FIRST);
foreach($menu_iterator as $entry) {
	echo $menu_iterator->getDepth() . $entry . "\n";
}
?>
===DONE===
--EXPECT--
0A
1B
2C
3D
2E
1F
===DONE===
