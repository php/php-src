--TEST--
The default value is false in the parent class method's signature.
--FILE--
<?php

interface MyDateTimeInterface extends DateTimeInterface
{
    public function diff(): DateInterval|false;
}
?>
--EXPECTF--
Fatal error: Declaration of MyDateTimeInterface::diff(): DateInterval|false must be compatible with DateTimeInterface::diff(DateTimeInterface $targetObject, bool $absolute = false): DateInterval|false in %s on line %d
