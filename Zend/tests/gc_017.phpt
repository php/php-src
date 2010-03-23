--TEST--
GC 017: GC and destructors with unset
--INI--
zend.enable_gc=1
--FILE--
<?php
class Node {
	public $name;
	public $children;
	public $parent;
	function __construct($name) {
		$this->name = $name;
		$this->children = array();
		$this->parent = null;
	}
	function insert($node) {
		$node->parent = $this;
		$this->children[] = $node;
	}
	function __destruct() {
		var_dump($this->name);
		unset($this->name);
		unset($this->children);
		unset($this->parent);
	}
}
$a = new Node('A');
$b = new Node('B');
$c = new Node('C');
$a->insert($b);
$a->insert($c);
unset($a);
unset($b);
unset($c);
var_dump(gc_collect_cycles());
echo "ok\n"
?>
--EXPECTF--
string(1) "%s"
string(1) "%s"
string(1) "%s"
int(10)
ok
