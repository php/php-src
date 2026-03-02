--TEST--
Bug #67118 crashes in DateTime when this used after failed __construct
--INI--
date.timezone=Europe/Berlin
--FILE--
<?php
class mydt extends datetime
{
    public function __construct($time = 'now', $tz = NULL, $format = NULL)
    {
        if (!empty($tz) && !is_object($tz)) {
            $tz = new DateTimeZone($tz);
        }
        try {
            @parent::__construct($time, $tz);
        } catch (Exception $e) {
            echo "Bad date" . $this->format("Y") . "\n";
        }
    }

};

new mydt("Funktionsansvarig rådgivning och juridik", "UTC");
?>
--EXPECTF--
Fatal error: Uncaught DateObjectError: Object of type mydt (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor in %s:%d
Stack trace:
#0 %s(%d): DateTime->format('Y')
#1 %s(%d): mydt->__construct(%s)
#2 {main}
  thrown in %s on line %d
