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
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $it = new myCachingIterator();
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $it = new myRecursiveCachingIterator();
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $it = new myParentIterator();
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $it = new myLimitIterator();
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $it = new myNoRewindIterator();
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ArgumentCountError: FilterIterator::__construct() expects exactly 1 argument, 0 given
ArgumentCountError: CachingIterator::__construct() expects at least 1 argument, 0 given
ArgumentCountError: RecursiveCachingIterator::__construct() expects at least 1 argument, 0 given
ArgumentCountError: ParentIterator::__construct() expects exactly 1 argument, 0 given
ArgumentCountError: LimitIterator::__construct() expects at least 1 argument, 0 given
ArgumentCountError: NoRewindIterator::__construct() expects exactly 1 argument, 0 given
