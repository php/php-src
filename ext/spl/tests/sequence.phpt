--TEST--
SPL: sequence
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
class c implements spl::iterator {

	public $max = 3;

	function new_iterator() {
		echo __METHOD__ . "\n";
		return new c_iter($this);
	}
}

class c_iter implements spl::sequence_assoc {

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
	function has_more() {
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
$i = $t->new_iterator(); 

$c_info = array(class_name($t) => array('inheits' => class_parents($t), 'implements' => class_implements($t)),
                class_name($i) => array('inheits' => class_parents($i), 'implements' => class_implements($i)));
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
c::new_iterator
Array
(
    [c] => Array
        (
            [inheits] => Array
                (
                )

            [implements] => Array
                (
                    [spl::iterator] => spl::iterator
                )

        )

    [c_iter] => Array
        (
            [inheits] => Array
                (
                )

            [implements] => Array
                (
                    [spl::sequence_assoc] => spl::sequence_assoc
                    [spl::forward_assoc] => spl::forward_assoc
                    [spl::assoc] => spl::assoc
                    [spl::forward] => spl::forward
                    [spl::sequence] => spl::sequence
                )

        )

)
c_iter::rewind
c_iter::has_more
c_iter::current
c_iter::key
object:0
c_iter::next
c_iter::has_more
c_iter::current
c_iter::key
object:1
c_iter::next
c_iter::has_more
c_iter::current
c_iter::key
object:2
c_iter::next
c_iter::has_more
c_iter::rewind
c_iter::has_more
c_iter::current
c_iter::key
object:1st=>0
c_iter::next
c_iter::has_more
c_iter::current
c_iter::key
object:2nd=>1
c_iter::next
c_iter::has_more
c_iter::current
c_iter::key
object:3rd=>2
c_iter::next
c_iter::has_more
Done