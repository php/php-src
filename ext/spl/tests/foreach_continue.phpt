--TEST--
SPL: foreach and break
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
class c_iter implements spl_forward {

	private $obj;
	private $num = 0;

	function __construct($obj) {
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
}
	
class c implements spl_iterator {

	public $max = 4;

	function newIterator() {
		echo __METHOD__ . "\n";
		return new c_iter($this);
	}
}

$t = new c();

foreach($t as $v) {
	if ($v == 0) {
		echo "continue outer\n";
		continue;
	}
	foreach($t as $w) {
		if ($w == 1) {
			echo "continue inner\n";
			continue;
		}
		if ($w == 2) {
			echo "break inner\n";
			break;
		}
		echo "double:$v:$w\n";
	}
	if ($v == 2) {
		echo "break outer\n";
		break;
	}
}

print "Done\n";
?>
--EXPECT--
c::newIterator
c_iter::hasMore = true
c_iter::current
continue outer
c_iter::next
c_iter::hasMore = true
c_iter::current
c::newIterator
c_iter::hasMore = true
c_iter::current
double:1:0
c_iter::next
c_iter::hasMore = true
c_iter::current
continue inner
c_iter::next
c_iter::hasMore = true
c_iter::current
break inner
c_iter::next
c_iter::hasMore = true
c_iter::current
c::newIterator
c_iter::hasMore = true
c_iter::current
double:2:0
c_iter::next
c_iter::hasMore = true
c_iter::current
continue inner
c_iter::next
c_iter::hasMore = true
c_iter::current
break inner
break outer
Done
