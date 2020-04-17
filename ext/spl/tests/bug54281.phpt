--TEST--
Bug #54281 (Crash in spl_recursive_it_rewind_ex)
--FILE--
<?php

class RecursiveArrayIteratorIterator extends RecursiveIteratorIterator {
    function __construct($it, $max_depth) { }
}
$it = new RecursiveArrayIteratorIterator(new RecursiveArrayIterator(array()), 2);

try {
    foreach($it as $k=>$v) { }
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
The object is in an invalid state as the parent constructor was not called
