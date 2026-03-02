--TEST--
The default value is an integer in the parent class method's signature.
--FILE--
<?php
class MyDateTime extends DateTime
{
    public function setTime(int $hour, int $minute, int $second = 0, bool $microsecond = false): DateTime
    {
    }
}
?>
--EXPECTF--
Fatal error: Declaration of MyDateTime::setTime(int $hour, int $minute, int $second = 0, bool $microsecond = false): DateTime must be compatible with DateTime::setTime(int $hour, int $minute, int $second = 0, int $microsecond = 0): DateTime in %s on line %d
