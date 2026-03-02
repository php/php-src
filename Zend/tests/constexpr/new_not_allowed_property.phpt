--TEST--
New not allowed in property
--FILE--
<?php

// New in (non-static) properties is a particularly tricky case. The initializer needs to be
// evaluated on each object construction. Currently, the places where this can happen is
// during object creation, or as part of the constructor. Doing this during object creation
// can issues for use-cases such as serialization or generally anything that is effectively
// based on newInstanceWithoutConstructor(). Handling this via the constructor is more
// promising, but requires injecting code in the constructor, which may require adding a
// constructor which is not explicitly declared, which may also require a child class to
// call a constructor that has not been explicitly declared, otherwise properties may be
// left uninitialized. A third option is another mechanism between object creation and
// constructor invocation. Overall, the best way to address this is not clear right now.

class Test {
    public $prop = new stdClass;
}

?>
--EXPECTF--
Fatal error: New expressions are not supported in this context in %s on line %d
