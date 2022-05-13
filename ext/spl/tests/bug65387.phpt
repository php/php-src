--TEST--
Bug #67387: Circular references in SPL iterators are not garbage collected
--FILE--
<?php

$it = new ArrayIterator([1, 2, 3]);
// Inner.
$it[] = $it;

// Callback
$it2 = new CallbackFilterIterator($it, function($elem) use(&$it2) {
    return true;
});

// Callback object
new class {
    private $it;
    public function __construct() {
        $it = new ArrayIterator([1, 2, 3]);
        $this->it = new CallbackFilterIterator($it, function($elem) {
            return true;
        });
    }
};

// Recursive callback
$it = new RecursiveArrayIterator([1, 2, 3]);
$it2 = new RecursiveCallbackFilterIterator($it, function($elem) use(&$it2) {
    return true;
});

// Cache
$it = new ArrayIterator();
$it2 = new CachingIterator($it, CachingIterator::FULL_CACHE);
$it2['x'] = $it2;
$it2->next();

// Recursive cache
$it = new RecursiveArrayIterator();
$it2 = new RecursiveCachingIterator($it, CachingIterator::FULL_CACHE);
$it2['x'] = $it2;
$it2->next();

// Append
$it = new ArrayIterator();
$it2 = new AppendIterator();
$it[] = $it2;
$it2->append($it);

// Recursive iterator
$it = new RecursiveArrayIterator([1, 2, 3]);
$it2 = new RecursiveIteratorIterator($it);
$it[] = $it2;

// Recursive iterator at level>0.
$it = new RecursiveArrayIterator([[1]]);
$it2 = new RecursiveIteratorIterator($it);
$it[] = $it2;
$it2->next();

?>
===DONE===
--EXPECT--
===DONE===
