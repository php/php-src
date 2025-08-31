--TEST--
Bug #54281 (Crash in spl_recursive_it_rewind_ex)
--FILE--
<?php

class RecursiveArrayIteratorIterator extends RecursiveIteratorIterator {
    function __construct($it, $max_depth) { }
}
$it = new RecursiveArrayIteratorIterator(new RecursiveArrayIterator(array()), 2);

foreach($it as $k=>$v) { }

?>
--EXPECTF--
Fatal error: Uncaught Error: Object is not initialized in %s:%d
Stack trace:
#0 {main}
  thrown in %s%ebug54281.php on line 8
