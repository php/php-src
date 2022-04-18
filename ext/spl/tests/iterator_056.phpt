--TEST--
SPL: Calling __construct(void) on class extending SPL iterator
--CREDITS--
Sebastian Schürmann
--FILE--
<?php

class myFilterIterator extends FilterIterator {
    function accept(): bool { }
}

class myCachingIterator extends CachingIterator { }

class myRecursiveCachingIterator extends RecursiveCachingIterator { }

class myParentIterator extends ParentIterator { }

class myLimitIterator extends LimitIterator { }

class myNoRewindIterator extends NoRewindIterator  {}

try {
    $it = new myFilterIterator();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $it = new myCachingIterator();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $it = new myRecursiveCachingIterator();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $it = new myParentIterator();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $it = new myLimitIterator();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $it = new myNoRewindIterator();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
FilterIterator::__construct() expects exactly 1 argument, 0 given
CachingIterator::__construct() expects at least 1 argument, 0 given
RecursiveCachingIterator::__construct() expects at least 1 argument, 0 given
ParentIterator::__construct() expects exactly 1 argument, 0 given
LimitIterator::__construct() expects at least 1 argument, 0 given
NoRewindIterator::__construct() expects exactly 1 argument, 0 given
