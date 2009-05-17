--TEST--
SPL: Heap and extract with parameter
--CREDITS--
Sean Burlington www.practicalweb.co.uk
TestFest London May 2009
--FILE--
<?php

class TestHeap extends SplHeap {

  function compare() {
    print "This shouldn't be printed";
  }
}

$testHeap = new TestHeap();



var_dump($testHeap->extract('test'));

?>
===DONE===
--EXPECTF--
Warning: SplHeap::extract() expects exactly 0 parameters, 1 given in %s on line 14
NULL
===DONE===
