--TEST--
__equals: Disallow comparison with an exception, but allow equality checks
--FILE--
<?php

class Test
{
    private $value;

    public function __construct($value)
    {
        $this->value = $value;
    }

    public function __compareTo($other)
    {
        throw new Exception("This object does not support comparison");
    }

    public function __equals($other)
    {
        return $other instanceof self && $this->value === $other->value;
    }
}

/**
 * This should call __equals which returns FALSE.
 */
var_dump(new Test(1) == new Test(2));

/**
 * This should call __equals which returns TRUE.
 */
var_dump(new Test(2) == new Test(2));

/**
 * This should call __compareTo which is explicitly unsupported.
 */
new Test(1) <=> new Test(1);

?>
--EXPECTF--
bool(false)
bool(true)

Fatal error: Uncaught Exception: %s:%d
Stack trace:
#0 %s: Test->__compareTo(Object(Test))
#1 {main}
  thrown in %s on line %d
