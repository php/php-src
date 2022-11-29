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
--EXPECT--
DONE
