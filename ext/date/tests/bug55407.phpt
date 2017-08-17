--TEST--
Bug #55407 (Impossible to prototype DateTime::createFromFormat)
--INI--
error_reporting=-1
--FILE--
<?php namespace melt\core;

class DateTime extends \DateTime {
    public static function createFromFormat($format, $time, \DateTimeZone $timezone = null) {
        return new DateTime(parent::createFromFormat($format, $time, $timezone));
    }
}

echo "DONE\n";
?>
--EXPECTF--
DONE
