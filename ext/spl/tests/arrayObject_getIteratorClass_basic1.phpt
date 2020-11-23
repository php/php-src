--TEST--
SPL: ArrayObject::getIteratorClass and ArrayObject::setIteratorClass basic functionality
--FILE--
<?php
class MyIterator extends ArrayIterator {

    function __construct() {
        $args = func_get_args();
        echo "   In " . __METHOD__ . "(" . implode(',', $args) . ")\n";
    }

    function rewind() {
        $args = func_get_args();
        echo "   In " . __METHOD__ . "(" . implode(',', $args) . ")\n";
        return parent::rewind();
    }

    function valid() {
        $args = func_get_args();
        echo "   In " . __METHOD__ . "(" . implode(',', $args) . ")\n";
        return parent::valid();
    }

    function current() {
        $args = func_get_args();
        echo "   In " . __METHOD__ . "(" . implode(',', $args) . ")\n";
        return parent::current();
    }

    function next() {
        $args = func_get_args();
        echo "   In " . __METHOD__ . "(" . implode(',', $args) . ")\n";
        return parent::next();
    }

    function key() {
        $args = func_get_args();
        echo "   In " . __METHOD__ . "(" . implode(',', $args) . ")\n";
        return parent::key();
    }
}

$ao = new ArrayObject(array('a'=>1,'b'=>2,'c'=>3), 0, "MyIterator");

echo "--> Access using MyIterator:\n";
var_dump($ao->getIteratorClass());
var_dump($ao->getIterator());
foreach($ao as $key=>$value) {
    echo "  $key=>$value\n";
}

echo "\n\n--> Access using ArrayIterator:\n";
var_dump($ao->setIteratorClass("ArrayIterator"));
var_dump($ao->getIteratorClass());
var_dump($ao->getIterator());
foreach($ao as $key=>$value) {
    echo "$key=>$value\n";
}

?>
--EXPECT--
--> Access using MyIterator:
string(10) "MyIterator"
object(MyIterator)#2 (1) {
  ["storage":"ArrayIterator":private]=>
  object(ArrayObject)#1 (1) {
    ["storage":"ArrayObject":private]=>
    array(3) {
      ["a"]=>
      int(1)
      ["b"]=>
      int(2)
      ["c"]=>
      int(3)
    }
  }
}
   In MyIterator::rewind()
   In MyIterator::valid()
   In MyIterator::current()
   In MyIterator::key()
  a=>1
   In MyIterator::next()
   In MyIterator::valid()
   In MyIterator::current()
   In MyIterator::key()
  b=>2
   In MyIterator::next()
   In MyIterator::valid()
   In MyIterator::current()
   In MyIterator::key()
  c=>3
   In MyIterator::next()
   In MyIterator::valid()


--> Access using ArrayIterator:
NULL
string(13) "ArrayIterator"
object(ArrayIterator)#3 (1) {
  ["storage":"ArrayIterator":private]=>
  object(ArrayObject)#1 (1) {
    ["storage":"ArrayObject":private]=>
    array(3) {
      ["a"]=>
      int(1)
      ["b"]=>
      int(2)
      ["c"]=>
      int(3)
    }
  }
}
a=>1
b=>2
c=>3
