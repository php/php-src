--TEST--
SPL: sequence
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
class c implements spl_iterator {

	public $max = 3;

	function newIterator() {
		echo __METHOD__ . "\n";
		return new c_iter($this);
	}
}

class c_iter implements spl_sequence_assoc {

	private $obj;
	private $num = 0;

	function __construct($obj) {
		$this->obj = $obj;
	}
	function rewind() {
		echo __METHOD__ . "\n";
		$this->num = 0;
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
		echo __METHOD__ . "\n";
		return $this->num < $this->obj->max;
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

$t = new c();
$i = $t->newIterator(); 

$c_info = array(get_class($t) => array('inheits' => class_parents($t), 'implements' => class_implements($t)),
                get_class($i) => array('inheits' => class_parents($i), 'implements' => class_implements($i)));
print_r($c_info);

foreach($i as $w) {
	echo "object:$w\n";
}

foreach($i as $v => $w) {
	echo "object:$v=>$w\n";
}

print "Done\n";
?>
--EXPECT--
c::newiterator
Array
(
    [c] => Array
        (
            [inheits] => Array
                (
                )

            [implements] => Array
                (
                    [spl_iterator] => spl_iterator
                )

        )

    [c_iter] => Array
        (
            [inheits] => Array
                (
                )

            [implements] => Array
                (
                    [spl_sequence_assoc] => spl_sequence_assoc
                    [spl_forward_assoc] => spl_forward_assoc
                    [spl_assoc] => spl_assoc
                    [spl_forward] => spl_forward
                    [spl_sequence] => spl_sequence
                )

        )

)
c_iter::rewind
c_iter::hasmore
c_iter::current
c_iter::key
object:0
c_iter::next
c_iter::hasmore
c_iter::current
c_iter::key
object:1
c_iter::next
c_iter::hasmore
c_iter::current
c_iter::key
object:2
c_iter::next
c_iter::hasmore
c_iter::rewind
c_iter::hasmore
c_iter::current
c_iter::key
object:1st=>0
c_iter::next
c_iter::hasmore
c_iter::current
c_iter::key
object:2nd=>1
c_iter::next
c_iter::hasmore
c_iter::current
c_iter::key
object:3rd=>2
c_iter::next
c_iter::hasmore
Done