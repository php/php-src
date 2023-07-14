--TEST--
DateTime/DateTimeImmutable compare handler
--FILE--
<?php
class MyDateTime extends DateTime
{
	function __construct()
	{
	}
}

$DT = new DateTime("2023-01-16 17:09:08");
$DTI = new DateTimeImmutable("2023-01-16 17:09:08");

$MDT = new MyDateTime();

var_dump($DT < $DTI);

try {
	var_dump($DT < $MDT);
} catch (\DateObjectError $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	var_dump($MDT < $DT);
} catch (\DateObjectError $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	var_dump($DTI < $MDT);
} catch (\DateObjectError $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
	var_dump($MDT < $DTI);
} catch (\DateObjectError $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(false)
DateObjectError: Trying to compare an incomplete DateTime or DateTimeImmutable object
DateObjectError: Trying to compare an incomplete DateTime or DateTimeImmutable object
DateObjectError: Trying to compare an incomplete DateTime or DateTimeImmutable object
DateObjectError: Trying to compare an incomplete DateTime or DateTimeImmutable object
