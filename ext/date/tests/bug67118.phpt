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

Warning: DateTime::format(): The DateTime object has not been correctly initialized by its constructor in %s on line %d
Bad date

