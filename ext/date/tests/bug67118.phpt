--TEST--
Bug #67118 php-cgi crashes regularly on IIS 7
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

		@parent::__construct($time, $tz);
	}

};

new mydt("Funktionsansvarig rådgivning och juridik", "UTC");
--EXPECTF--
Fatal error: Uncaught exception 'Exception' with message 'DateTime::__construct(): Failed to parse time string (Funktionsansvarig rådgivning och juridik) at position 0 (F): The timezone could not be found in the database' in %sbug67118.php:%d
Stack trace:
#0 %sbug67118.php(%d): DateTime->__construct('Funktionsansvar...', Object(DateTimeZone))
#1 %sbug67118.php(%d): mydt->__construct('Funktionsansvar...', 'UTC')
#2 {main}
  thrown in %sbug67118.php on line %d
