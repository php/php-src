--TEST--
Bug GH-11455 (PHP 8.2 Segmentation fault on nesbot/carbon)
--FILE--
<?php
class MyDateTimeImmutable extends DateTimeImmutable {
    public function __construct(
        string $datetime = "now",
        ?DateTimeZone $timezone = null,
        public ?stdClass $myProperty = null,
    ) {
        parent::__construct($datetime, $timezone);
    }
}

$datetime = new MyDateTimeImmutable('2022-12-22T11:26:00Z', myProperty: new stdClass);
$datetime->myProperty->field = str_repeat("hello", 3);
$serialized = serialize($datetime);
var_dump($datetime->myProperty);
$unserialized = unserialize($serialized);
var_dump($unserialized);
?>
--EXPECT--
object(stdClass)#2 (1) {
  ["field"]=>
  string(15) "hellohellohello"
}
object(MyDateTimeImmutable)#3 (4) {
  ["myProperty"]=>
  object(stdClass)#4 (1) {
    ["field"]=>
    string(15) "hellohellohello"
  }
  ["date"]=>
  string(26) "2022-12-22 11:26:00.000000"
  ["timezone_type"]=>
  int(2)
  ["timezone"]=>
  string(1) "Z"
}
