--TEST--
SPL: ArrayObject with bad iterator class.
--FILE--
<?php
try {
  $ao = new ArrayObject(array('a'=>1,'b'=>2,'c'=>3));
  $ao->setIteratorClass("nonExistentClass");
  foreach($ao as $key=>$value) {
    echo "  $key=>$value\n";
  }
} catch (TypeError $e) {
    var_dump($e->getMessage());
}

try {
  $ao = new ArrayObject(array('a'=>1,'b'=>2,'c'=>3));
  $ao->setIteratorClass("stdClass");
  foreach($ao as $key=>$value) {
    echo "  $key=>$value\n";
  }
} catch (TypeError $e) {
    var_dump($e->getMessage());
}


try {
  $ao = new ArrayObject(array('a'=>1,'b'=>2,'c'=>3), 0, "nonExistentClass");
  foreach($ao as $key=>$value) {
    echo "  $key=>$value\n";
  }
} catch (TypeError $e) {
    var_dump($e->getMessage());
}

try {
  $ao = new ArrayObject(array('a'=>1,'b'=>2,'c'=>3), 0, "stdClass");
  foreach($ao as $key=>$value) {
    echo "  $key=>$value\n";
  }
} catch (TypeError $e) {
    var_dump($e->getMessage());
}

?>
--EXPECT--
string(135) "ArrayObject::setIteratorClass() expects argument #1 ($iteratorClass) to be a class name derived from Iterator, 'nonExistentClass' given"
string(127) "ArrayObject::setIteratorClass() expects argument #1 ($iteratorClass) to be a class name derived from Iterator, 'stdClass' given"
string(131) "ArrayObject::__construct() expects argument #3 ($iterator_class) to be a class name derived from Iterator, 'nonExistentClass' given"
string(123) "ArrayObject::__construct() expects argument #3 ($iterator_class) to be a class name derived from Iterator, 'stdClass' given"
