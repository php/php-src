--TEST--
The default value is a constant in the parent class method's signature.
--FILE--
<?php
class MyDateTimeZone extends DateTimeZone
{
    public function getTransitions()
    {
    }
}
--EXPECTF--
Fatal error: Declaration of MyDateTimeZone::getTransitions() must be compatible with DateTimeZone::getTransitions(int $timestamp_begin = PHP_INT_MIN, int $timestamp_end = PHP_INT_MAX) in %s on line %d
