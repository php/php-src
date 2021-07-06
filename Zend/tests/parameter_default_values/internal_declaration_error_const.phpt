--TEST--
The default value is a constant in the parent class method's signature.
--FILE--
<?php
class MyDateTimeZone extends DateTimeZone
{
    public function getTransitions(): array|false
    {
    }
}
?>
--EXPECTF--
Fatal error: Declaration of MyDateTimeZone::getTransitions(): array|false must be compatible with DateTimeZone::getTransitions(int $timestampBegin = PHP_INT_MIN, int $timestampEnd = PHP_INT_MAX): array|false in %s on line %d
