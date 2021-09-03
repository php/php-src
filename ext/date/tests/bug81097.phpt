--TEST--
Bug #81097: DateTimeZone silently falls back to UTC when providing an offset with seconds
--FILE--
<?php
try {
	new DatetimeZone('+01:45:30');
} catch (Exception $e) {
	echo $e->getMessage();
}
?>
--EXPECT--
DateTimeZone::__construct(): Unknown or bad timezone (+01:45:30)
