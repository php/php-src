--TEST--
SPL: foreach and iterator
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
class c_iter implements spl_forward_assoc {

	private $obj;
	private $num = 0;

	function __construct($obj) {
		echo __METHOD__ . "\n";
		$this->num = 0;
		$this->obj = $obj;
	}
	function current() {
		echo __METHOD__ . "\n";
		return $this->num;
	}
	function next() {
		echo __METHOD__ . "\n";
		$this->num++;
	}
	function hasMore() {
		$more = $this->num < $this->obj->max;
		echo __METHOD__ . ' = ' .($more ? 'true' : 'false') . "\n";
		return $more;
	}
	function key() {
		echo __METHOD__ . "\n";
		switch($this->num) {
			case 0: return "1st";
			case 1: return "2nd";
			case 2: return "3rd";
			default: return "???";
		}
	}
}
	
class c implements spl_iterator {

	public $max = 3;

	function newIterator() {
		echo __METHOD__ . "\n";
		return new c_iter($this);
	}
}

$t = new c();

for ($iter = $t->newIterator(); $iter->hasMore(); $iter->next()) {
	echo $iter->current() . "\n";
}

$a = array(0,1,2);
foreach($a as $v) {
	echo "array:$v\n";
}

foreach($t as $v) {
	echo "object:$v\n";
}

foreach($t as $v) {
	foreach($t as $w) {
		echo "double:$v:$w\n";
	}
}

foreach($t as $i => $v) {
	echo "object:$i=>$v\n";
}

print "Done\n";
?>
--EXPECT--
c::newIterator
c_iter::__construct
c_iter::hasMore = true
c_iter::current
0
c_iter::next
c_iter::hasMore = true
c_iter::current
1
c_iter::next
c_iter::hasMore = true
c_iter::current
2
c_iter::next
c_iter::hasMore = false
array:0
array:1
array:2
c::newIterator
c_iter::__construct
c_iter::hasMore = true
c_iter::current
c_iter::key
object:0
c_iter::next
c_iter::hasMore = true
c_iter::current
c_iter::key
object:1
c_iter::next
c_iter::hasMore = true
c_iter::current
c_iter::key
object:2
c_iter::next
c_iter::hasMore = false
c::newIterator
c_iter::__construct
c_iter::hasMore = true
c_iter::current
c_iter::key
c::newIterator
c_iter::__construct
c_iter::hasMore = true
c_iter::current
c_iter::key
double:0:0
c_iter::next
c_iter::hasMore = true
c_iter::current
c_iter::key
double:0:1
c_iter::next
c_iter::hasMore = true
c_iter::current
c_iter::key
double:0:2
c_iter::next
c_iter::hasMore = false
c_iter::next
c_iter::hasMore = true
c_iter::current
c_iter::key
c::newIterator
c_iter::__construct
c_iter::hasMore = true
c_iter::current
c_iter::key
double:1:0
c_iter::next
c_iter::hasMore = true
c_iter::current
c_iter::key
double:1:1
c_iter::next
c_iter::hasMore = true
c_iter::current
c_iter::key
double:1:2
c_iter::next
c_iter::hasMore = false
c_iter::next
c_iter::hasMore = true
c_iter::current
c_iter::key
c::newIterator
c_iter::__construct
c_iter::hasMore = true
c_iter::current
c_iter::key
double:2:0
c_iter::next
c_iter::hasMore = true
c_iter::current
c_iter::key
double:2:1
c_iter::next
c_iter::hasMore = true
c_iter::current
c_iter::key
double:2:2
c_iter::next
c_iter::hasMore = false
c_iter::next
c_iter::hasMore = false
c::newIterator
c_iter::__construct
c_iter::hasMore = true
c_iter::current
c_iter::key
object:1st=>0
c_iter::next
c_iter::hasMore = true
c_iter::current
c_iter::key
object:2nd=>1
c_iter::next
c_iter::hasMore = true
c_iter::current
c_iter::key
object:3rd=>2
c_iter::next
c_iter::hasMore = false
Done
