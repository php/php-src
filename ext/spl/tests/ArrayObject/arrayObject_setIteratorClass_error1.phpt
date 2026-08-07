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
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
  $ao = new ArrayObject(array('a'=>1,'b'=>2,'c'=>3));
  $ao->setIteratorClass("stdClass");
  foreach($ao as $key=>$value) {
    echo "  $key=>$value\n";
  }
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}


try {
  $ao = new ArrayObject(array('a'=>1,'b'=>2,'c'=>3), 0, "nonExistentClass");
  foreach($ao as $key=>$value) {
    echo "  $key=>$value\n";
  }
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
  $ao = new ArrayObject(array('a'=>1,'b'=>2,'c'=>3), 0, "stdClass");
  foreach($ao as $key=>$value) {
    echo "  $key=>$value\n";
  }
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: ArrayObject::setIteratorClass(): Argument #1 ($iteratorClass) must be a class name derived from ArrayIterator, nonExistentClass given
TypeError: ArrayObject::setIteratorClass(): Argument #1 ($iteratorClass) must be a class name derived from ArrayIterator, stdClass given
TypeError: ArrayObject::__construct(): Argument #3 ($iteratorClass) must be a class name derived from ArrayIterator, nonExistentClass given
TypeError: ArrayObject::__construct(): Argument #3 ($iteratorClass) must be a class name derived from ArrayIterator, stdClass given
