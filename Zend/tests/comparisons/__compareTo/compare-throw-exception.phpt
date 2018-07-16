--TEST--
__compareTo: Throwing exception is caught
--FILE--
<?php

/**
 * Check that throwing an exception in the comparison function is caught.
 */
class BrokenComparison
{
    public function __compareTo($other)
    {
        throw new Exception();
    }
}

new BrokenComparison <=> new BrokenComparison;

?>
--EXPECTF--

Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s: BrokenComparison->__compareTo(Object(BrokenComparison))
#1 {main}
  thrown in %s on line %d
