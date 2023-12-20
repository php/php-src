--TEST--
Tests for DateTime[Immutable]::[get|set]Microseconds
--FILE--
<?php

class MyDateTime extends DateTime {};
class MyDateTimeImmutable extends DateTimeImmutable {};

$microsecondsList = array(
    0,
    999999,
    -1,
    1000000
);

$dt = new DateTime('2023-04-05T06:07:08.901234+00:00');
$dti = new DateTimeImmutable('2023-04-05T06:07:08.901234+00:00');
$myDt = new MyDateTime('2023-04-05T06:07:08.901234+00:00');
$myDti = new MyDateTimeImmutable('2023-04-05T06:07:08.901234+00:00');

echo 'DateTime::getMicroseconds(): ' . var_export($dt->getMicroseconds(), true) . "\n";
var_dump($dt);

echo 'DateTimeImmutable::getMicroseconds(): ' . var_export($dti->getMicroseconds(), true) . "\n";
var_dump($dti);

echo 'MyDateTime::getMicroseconds(): ' . var_export($myDt->getMicroseconds(), true) . "\n";
var_dump($myDt);

echo 'MyDateTimeImmutable::getMicroseconds(): ' . var_export($myDti->getMicroseconds(), true) . "\n";
var_dump($myDti);

foreach ($microsecondsList as $microseconds) {
    echo "##################################\n";

    echo 'DateTime::setMicroseconds('.var_export($microseconds, true).'): ';
    try {
        var_dump($dt->setMicroseconds($microseconds));
    } catch (Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
    }
    echo 'DateTime::getMicroseconds(): ' . var_export($dt->getMicroseconds(), true) . "\n";

    echo 'DateTimeImmutable::setMicroseconds('.var_export($microseconds, true).'): ';
    try {
        var_dump($dti->setMicroseconds($microseconds));
    } catch (Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
    }
    echo 'DateTimeImmutable::getMicroseconds(): ' . var_export($dti->getMicroseconds(), true) . "\n";

    echo 'MyDateTime::setMicroseconds('.var_export($microseconds, true).'): ';
    try {
        var_dump($myDt->setMicroseconds($microseconds));
    } catch (Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
    }
    echo 'MyDateTime::getMicroseconds(): ' . var_export($myDt->getMicroseconds(), true) . "\n";

    echo 'MyDateTimeImmutable::setMicroseconds('.var_export($microseconds, true).'): ';
    try {
        var_dump($myDti->setMicroseconds($microseconds));
    } catch (Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
    }
    echo 'MyDateTimeImmutable::getMicroseconds(): ' . var_export($myDti->getMicroseconds(), true) . "\n";
}
?>
--EXPECTF--
DateTime::getMicroseconds(): 901234
object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.901234"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::getMicroseconds(): 901234
object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.901234"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
MyDateTime::getMicroseconds(): 901234
object(MyDateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.901234"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
MyDateTimeImmutable::getMicroseconds(): 901234
object(MyDateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.901234"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
##################################
DateTime::setMicroseconds(0): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::getMicroseconds(): 0
DateTimeImmutable::setMicroseconds(0): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::getMicroseconds(): 901234
MyDateTime::setMicroseconds(0): object(MyDateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
MyDateTime::getMicroseconds(): 0
MyDateTimeImmutable::setMicroseconds(0): object(MyDateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.000000"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
MyDateTimeImmutable::getMicroseconds(): 901234
##################################
DateTime::setMicroseconds(999999): object(DateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.999999"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTime::getMicroseconds(): 999999
DateTimeImmutable::setMicroseconds(999999): object(DateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.999999"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
DateTimeImmutable::getMicroseconds(): 901234
MyDateTime::setMicroseconds(999999): object(MyDateTime)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.999999"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
MyDateTime::getMicroseconds(): 999999
MyDateTimeImmutable::setMicroseconds(999999): object(MyDateTimeImmutable)#%d (3) {
  ["date"]=>
  string(26) "2023-04-05 06:07:08.999999"
  ["timezone_type"]=>
  int(1)
  ["timezone"]=>
  string(6) "+00:00"
}
MyDateTimeImmutable::getMicroseconds(): 901234
##################################
DateTime::setMicroseconds(-1): DateRangeError: DateTime::setMicroseconds(): Argument #1 ($microseconds) must be between 0 and 999999, -1 given
DateTime::getMicroseconds(): 999999
DateTimeImmutable::setMicroseconds(-1): DateRangeError: DateTimeImmutable::setMicroseconds(): Argument #1 ($microseconds) must be between 0 and 999999, -1 given
DateTimeImmutable::getMicroseconds(): 901234
MyDateTime::setMicroseconds(-1): DateRangeError: DateTime::setMicroseconds(): Argument #1 ($microseconds) must be between 0 and 999999, -1 given
MyDateTime::getMicroseconds(): 999999
MyDateTimeImmutable::setMicroseconds(-1): DateRangeError: DateTimeImmutable::setMicroseconds(): Argument #1 ($microseconds) must be between 0 and 999999, -1 given
MyDateTimeImmutable::getMicroseconds(): 901234
##################################
DateTime::setMicroseconds(1000000): DateRangeError: DateTime::setMicroseconds(): Argument #1 ($microseconds) must be between 0 and 999999, 1000000 given
DateTime::getMicroseconds(): 999999
DateTimeImmutable::setMicroseconds(1000000): DateRangeError: DateTimeImmutable::setMicroseconds(): Argument #1 ($microseconds) must be between 0 and 999999, 1000000 given
DateTimeImmutable::getMicroseconds(): 901234
MyDateTime::setMicroseconds(1000000): DateRangeError: DateTime::setMicroseconds(): Argument #1 ($microseconds) must be between 0 and 999999, 1000000 given
MyDateTime::getMicroseconds(): 999999
MyDateTimeImmutable::setMicroseconds(1000000): DateRangeError: DateTimeImmutable::setMicroseconds(): Argument #1 ($microseconds) must be between 0 and 999999, 1000000 given
MyDateTimeImmutable::getMicroseconds(): 901234
