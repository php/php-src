--TEST--
DateTimeImmutable::createFromInterface exception
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

class MyDateTimeImmutable extends DateTimeImmutable
{
	function __construct()
	{
	}
}

$i = new MyDateTime();
try {
	$m = DateTimeImmutable::createFromInterface( $i );
} catch (\DateObjectError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$i = new MyDateTimeImmutable();
try {
	$m = DateTimeImmutable::createFromInterface( $i );
} catch (\DateObjectError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
DateObjectError: Object of type MyDateTime (inheriting DateTime) has not been correctly initialized by calling parent::__construct() in its constructor
DateObjectError: Object of type MyDateTimeImmutable (inheriting DateTimeImmutable) has not been correctly initialized by calling parent::__construct() in its constructor
