--TEST--
Tests for DateTime[Immutable]::[get|set]Microseconds
--FILE--
<?php

class MyDateTime extends DateTime {};
class MyDateTimeImmutable extends DateTimeImmutable {};

$microsecondList = array(
    0,
    999999,
    -1,
    1000000
);

$dt = new DateTime('2023-04-05T06:07:08.901234+00:00');
$dti = new DateTimeImmutable('2023-04-05T06:07:08.901234+00:00');
$myDt = new MyDateTime('2023-04-05T06:07:08.901234+00:00');
$myDti = new MyDateTimeImmutable('2023-04-05T06:07:08.901234+00:00');

echo 'DateTime::getMicrosecond(): ' . var_export($dt->getMicrosecond(), true) . "\n";
var_dump($dt);

echo 'DateTimeImmutable::getMicrosecond(): ' . var_export($dti->getMicrosecond(), true) . "\n";
var_dump($dti);

echo 'MyDateTime::getMicrosecond(): ' . var_export($myDt->getMicrosecond(), true) . "\n";
var_dump($myDt);

echo 'MyDateTimeImmutable::getMicrosecond(): ' . var_export($myDti->getMicrosecond(), true) . "\n";
var_dump($myDti);

foreach ($microsecondList as $microsecond) {
    echo "##################################\n";

    echo 'DateTime::setMicrosecond('.var_export($microsecond, true).'): ';
    try {
        var_dump($dt->setMicrosecond($microsecond));
    } catch (Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
    }
    echo 'DateTime::getMicrosecond(): ' . var_export($dt->getMicrosecond(), true) . "\n";

    echo 'DateTimeImmutable::setMicrosecond('.var_export($microsecond, true).'): ';
    try {
        var_dump($dti->setMicrosecond($microsecond));
    } catch (Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
    }
    echo 'DateTimeImmutable::getMicrosecond(): ' . var_export($dti->getMicrosecond(), true) . "\n";

    echo 'MyDateTime::setMicrosecond('.var_export($microsecond, true).'): ';
    try {
        var_dump($myDt->setMicrosecond($microsecond));
    } catch (Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
    }
    echo 'MyDateTime::getMicrosecond(): ' . var_export($myDt->getMicrosecond(), true) . "\n";

    echo 'MyDateTimeImmutable::setMicrosecond('.var_export($microsecond, true).'): ';
    try {
        var_dump($myDti->setMicrosecond($microsecond));
    } catch (Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
    }
    echo 'MyDateTimeImmutable::getMicrosecond(): ' . var_export($myDti->getMicrosecond(), true) . "\n";
}
?>
--EXPECTF--
DateTime::getMicrosecond(): 901234
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.901234"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::getMicrosecond(): 901234
object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.901234"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
MyDateTime::getMicrosecond(): 901234
object(MyDateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.901234"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
MyDateTimeImmutable::getMicrosecond(): 901234
object(MyDateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.901234"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
##################################
DateTime::setMicrosecond(0): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::getMicrosecond(): 0
DateTimeImmutable::setMicrosecond(0): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::getMicrosecond(): 901234
MyDateTime::setMicrosecond(0): object(MyDateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
MyDateTime::getMicrosecond(): 0
MyDateTimeImmutable::setMicrosecond(0): object(MyDateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
MyDateTimeImmutable::getMicrosecond(): 901234
##################################
DateTime::setMicrosecond(999999): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.999999"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::getMicrosecond(): 999999
DateTimeImmutable::setMicrosecond(999999): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.999999"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::getMicrosecond(): 901234
MyDateTime::setMicrosecond(999999): object(MyDateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.999999"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
MyDateTime::getMicrosecond(): 999999
MyDateTimeImmutable::setMicrosecond(999999): object(MyDateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.999999"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
MyDateTimeImmutable::getMicrosecond(): 901234
##################################
DateTime::setMicrosecond(-1): DateRangeError: DateTime::setMicrosecond(): Argument #1 ($microsecond) must be between 0 and 999999, -1 given
DateTime::getMicrosecond(): 999999
DateTimeImmutable::setMicrosecond(-1): DateRangeError: DateTimeImmutable::setMicrosecond(): Argument #1 ($microsecond) must be between 0 and 999999, -1 given
DateTimeImmutable::getMicrosecond(): 901234
MyDateTime::setMicrosecond(-1): DateRangeError: DateTime::setMicrosecond(): Argument #1 ($microsecond) must be between 0 and 999999, -1 given
MyDateTime::getMicrosecond(): 999999
MyDateTimeImmutable::setMicrosecond(-1): DateRangeError: DateTimeImmutable::setMicrosecond(): Argument #1 ($microsecond) must be between 0 and 999999, -1 given
MyDateTimeImmutable::getMicrosecond(): 901234
##################################
DateTime::setMicrosecond(1000000): DateRangeError: DateTime::setMicrosecond(): Argument #1 ($microsecond) must be between 0 and 999999, 1000000 given
DateTime::getMicrosecond(): 999999
DateTimeImmutable::setMicrosecond(1000000): DateRangeError: DateTimeImmutable::setMicrosecond(): Argument #1 ($microsecond) must be between 0 and 999999, 1000000 given
DateTimeImmutable::getMicrosecond(): 901234
MyDateTime::setMicrosecond(1000000): DateRangeError: DateTime::setMicrosecond(): Argument #1 ($microsecond) must be between 0 and 999999, 1000000 given
MyDateTime::getMicrosecond(): 999999
MyDateTimeImmutable::setMicrosecond(1000000): DateRangeError: DateTimeImmutable::setMicrosecond(): Argument #1 ($microsecond) must be between 0 and 999999, 1000000 given
MyDateTimeImmutable::getMicrosecond(): 901234
