--TEST--
GC 046: Leak in User Iterator
--INI--
zend.enable_gc=1
--FILE--
<?php
class Action {
    private $iterator;
    function __construct() {
        $this->iterator = new ArrayIterator($this);
    }
    function filter() {
        $this->iterator = new CallbackFilterIterator($this->iterator, fn() => true);
        $this->iterator->rewind();
    }
}

$action=new Action;
$action->filter();
$action->filter();
?>
DONE
--EXPECTF--
Deprecated: ArrayIterator::__construct(): Using an object as a backing array for ArrayIterator is deprecated, as it allows violating class constraints and invariants in %s on line %d
DONE
