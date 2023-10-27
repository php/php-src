--TEST--
Bug GH-11310: __debugInfo() does nothing on classes extending DateTime
--INI--
date.timezone=UTC
--FILE--
<?php

class MyDateTime extends DateTime {
    public function __debugInfo(): array {
        return ['child' => '42', 'parent' => count(parent::__debugInfo())];
    }
}

class MyDateTimeImmutable extends DateTimeImmutable {
    public function __debugInfo(): array {
        return ['child' => '42', 'parent' => count(parent::__debugInfo())];
    }
}

class MyDateTimeZone extends DateTimeZone {
    public function __debugInfo(): array {
        return ['child' => '42', 'parent' => count(parent::__debugInfo())];
    }
}

var_dump(new DateTime('2023-10-26 21:23:05'));
var_dump(new DateTimeImmutable('2023-10-26 21:23:05'));
var_dump(new MyDateTime('2023-10-26 21:23:05'));
var_dump(new MyDateTimeImmutable('2023-10-26 21:23:05'));
?>
--EXPECT--
object(DateTime)#1 (3) {
  ["date"]=>
  string(26) "2023-10-26 21:23:05.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
object(DateTimeImmutable)#1 (3) {
  ["date"]=>
  string(26) "2023-10-26 21:23:05.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
object(MyDateTime)#1 (2) {
  ["child"]=>
  string(2) "42"
  ["parent"]=>
  int(3)
}
object(MyDateTimeImmutable)#1 (2) {
  ["child"]=>
  string(2) "42"
  ["parent"]=>
  int(3)
}
