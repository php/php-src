--TEST--
Bug GH-10152 (Custom properties of DateTimeImmutable child classes are not serialized)
--FILE--
<?php

class MyDateTimeImmutable extends DateTimeImmutable {
    public function __construct(
        string $datetime = "now",
        ?DateTimeZone $timezone = null,
        public ?bool $myProperty = null,
    ) {
        parent::__construct($datetime, $timezone);
    }
}

$datetime = new MyDateTimeImmutable('2022-12-22T11:26:00Z', myProperty: true);
$serialized = serialize($datetime);
$unserialized = unserialize($serialized);
var_dump($unserialized->myProperty);
?>
--EXPECT--
bool(true)
