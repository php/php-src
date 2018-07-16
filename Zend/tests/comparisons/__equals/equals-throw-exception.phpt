--TEST--
__equals: Throwing exception is caught
--FILE--
<?php

/**
 * Check that throwing an exception in the equality function is caught.
 */
class BrokenEquatable
{
    public function __equals($other)
    {
        throw new Exception();
    }
}

new BrokenEquatable == new BrokenEquatable;

?>
--EXPECTF--

Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s: BrokenEquatable->__equals(Object(BrokenEquatable))
#1 {main}
  thrown in %s on line %d
