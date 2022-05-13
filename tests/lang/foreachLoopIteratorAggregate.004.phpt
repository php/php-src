--TEST--
Duplicate of zend test tests/classes/iterators_002.phpt without expected output from destructor
--FILE--
<?php
class c_iter implements Iterator {

    private $obj;
    private $num = 0;

    function __construct($obj) {
        echo __METHOD__ . "\n";
        $this->obj = $obj;
    }
    function rewind(): void {
        echo __METHOD__ . "\n";
        $this->num = 0;
    }
    function valid(): bool {
        $more = $this->num < $this->obj->max;
        echo __METHOD__ . ' = ' .($more ? 'true' : 'false') . "\n";
        return $more;
    }
    function current(): mixed {
        echo __METHOD__ . "\n";
        return $this->num;
    }
    function next(): void {
        echo __METHOD__ . "\n";
        $this->num++;
    }
    function key(): mixed {
        echo __METHOD__ . "\n";
        switch($this->num) {
            case 0: return "1st";
            case 1: return "2nd";
            case 2: return "3rd";
            default: return "???";
        }
    }
    function __destruct() {
    }
}

class c implements IteratorAggregate {

    public $max = 3;

    function getIterator(): Traversable {
        echo __METHOD__ . "\n";
        return new c_iter($this);
    }
    function __destruct() {
    }
}

$t = new c();

foreach($t as $k => $v) {
    foreach($t as $w) {
        echo "double:$v:$w\n";
        break;
    }
}

unset($t);

?>
--EXPECT--
c::getIterator
c_iter::__construct
c_iter::rewind
c_iter::valid = true
c_iter::current
c_iter::key
c::getIterator
c_iter::__construct
c_iter::rewind
c_iter::valid = true
c_iter::current
double:0:0
c_iter::next
c_iter::valid = true
c_iter::current
c_iter::key
c::getIterator
c_iter::__construct
c_iter::rewind
c_iter::valid = true
c_iter::current
double:1:0
c_iter::next
c_iter::valid = true
c_iter::current
c_iter::key
c::getIterator
c_iter::__construct
c_iter::rewind
c_iter::valid = true
c_iter::current
double:2:0
c_iter::next
c_iter::valid = false
