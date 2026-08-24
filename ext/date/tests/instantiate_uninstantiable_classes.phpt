--TEST--
Instantiating uninstantiable classes via static constructors
--FILE--
<?php

abstract class MyDatePeriod extends DatePeriod {
    public abstract function foo();
}

abstract class MyDateTime extends DateTime {
    public abstract function foo();
}

abstract class MyDateTimeImmutable extends DateTimeImmutable {
    public abstract function foo();
}

try {
    MyDatePeriod::createFromISO8601String('R5');
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    MyDateTime::createFromFormat('Y-m-d', '2025-01-01');
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    MyDateTime::createFromImmutable(new DateTimeImmutable());
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    MyDateTime::createFromInterface(new DateTimeImmutable());
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    MyDateTime::createFromTimestamp(0);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    MyDateTimeImmutable::createFromFormat('Y-m-d', '2025-01-01');
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    MyDateTimeImmutable::createFromMutable(new DateTime());
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    MyDateTimeImmutable::createFromInterface(new DateTime());
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    MyDateTimeImmutable::createFromTimestamp(0);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot instantiate abstract class MyDatePeriod
Error: Cannot instantiate abstract class MyDateTime
Error: Cannot instantiate abstract class MyDateTime
Error: Cannot instantiate abstract class MyDateTime
Error: Cannot instantiate abstract class MyDateTime
Error: Cannot instantiate abstract class MyDateTimeImmutable
Error: Cannot instantiate abstract class MyDateTimeImmutable
Error: Cannot instantiate abstract class MyDateTimeImmutable
Error: Cannot instantiate abstract class MyDateTimeImmutable
