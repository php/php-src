--TEST--
Bug #78139 (timezone_open accepts invalid timezone string argument)
--FILE--
<?php
$strings = [
	"x",
	"x UTC",
	"xx UTC",
	"xUTC",
	"UTCx",
	"UTC xx",
];

foreach ($strings as $string)
{
	echo "Parsing '{$string}':\n";

	$tz = timezone_open($string);
	var_dump($tz);

	try {
		$tz = new \DateTimeZone($string);
	} catch (Exception $e) {
		echo $e->getMessage(), "\n";
	}

	echo "\n\n";
}
?>
--EXPECTF--
Parsing 'x':
object(DateTimeZone)#1 (2) {
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(1) "X"
}


Parsing 'x UTC':

Warning: timezone_open(): Unknown or bad timezone (x UTC) in %sbug78139.php on line %d
bool(false)
DateTimeZone::__construct(): Unknown or bad timezone (x UTC)


Parsing 'xx UTC':

Warning: timezone_open(): Unknown or bad timezone (xx UTC) in %sbug78139.php on line %d
bool(false)
DateTimeZone::__construct(): Unknown or bad timezone (xx UTC)


Parsing 'xUTC':

Warning: timezone_open(): Unknown or bad timezone (xUTC) in %sbug78139.php on line %d
bool(false)
DateTimeZone::__construct(): Unknown or bad timezone (xUTC)


Parsing 'UTCx':

Warning: timezone_open(): Unknown or bad timezone (UTCx) in %sbug78139.php on line %d
bool(false)
DateTimeZone::__construct(): Unknown or bad timezone (UTCx)


Parsing 'UTC xx':

Warning: timezone_open(): Unknown or bad timezone (UTC xx) in %sbug78139.php on line %d
bool(false)
DateTimeZone::__construct(): Unknown or bad timezone (UTC xx)

