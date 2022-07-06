--TEST--
The default value is false in the parent class method's signature.
--FILE--
<?php

interface MyDateTimeInterface extends DateTimeInterface
{
    public function diff();
}
?>
--EXPECTF--
Fatal error: Declaration of MyDateTimeInterface::diff() must be compatible with DateTimeInterface::diff(DateTimeInterface $targetObject, bool $absolute = false) in %s on line %d
