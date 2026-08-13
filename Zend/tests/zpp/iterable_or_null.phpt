--TEST--
Test Z_PARAM_ITERABLE() and Z_PARAM_ITERABLE_OR_NULL
--EXTENSIONS--
zend_test
--FILE--
<?php

try {
	  var_dump(zend_iterable("string"));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
	  var_dump(zend_iterable(1));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
	  var_dump(zend_iterable(null));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}


zend_iterable([]);
zend_iterable([], []);

$iterator = new ArrayIterator([]);
zend_iterable($iterator);
zend_iterable($iterator, $iterator);
zend_iterable($iterator, null);

try {
	  var_dump(zend_iterable([], "string"));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
zend_iterable(): Argument #1 ($arg1) must be of type Traversable|array, string given
zend_iterable(): Argument #1 ($arg1) must be of type Traversable|array, int given
zend_iterable(): Argument #1 ($arg1) must be of type Traversable|array, null given
zend_iterable(): Argument #2 ($arg2) must be of type Traversable|array|null, string given

