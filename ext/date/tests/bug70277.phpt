--TEST--
Bug #70277 (new DateTimeZone($foo) is ignoring text after null byte)
--FILE--
<?php
$timezone = "Europe/Zurich\0Foo";
var_dump(timezone_open($timezone));
var_dump(new DateTimeZone($timezone));
?>
--EXPECTF--
Warning: timezone_open(): Timezone must not contain null bytes in %sbug70277.php on line %d
bool(false)

Fatal error: Uncaught Exception: DateTimeZone::__construct(): Timezone must not contain null bytes in %sbug70277.php:%d
Stack trace:
#0 %sbug70277.php(%d): DateTimeZone->__construct('Europe/Zurich\x00F...')
#1 {main}
  thrown in %sbug70277.php on line %d
