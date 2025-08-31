--TEST--
DateTimeImmutable::createFromMutable exception
--INI--
date.timezone=Europe/London
--FILE--
<?php
class MyDateTime extends DateTime
{
	function __construct()
	{
	}
}

$i = new MyDateTime();

try {
	$m = DateTimeImmutable::createFromMutable( $i );
} catch (\DateObjectError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
